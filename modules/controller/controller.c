/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>

#include <errno.h>
#include "controller.h"
#include "controller/time.h"
#include "log.h"
#include "xtimer.h"
#include "gui.h"
#include "ts_event.h"

#define CONTROLLER_THREAD_NAME    "controller"
#define CONTROLLER_THREAD_PRIO    6
#define CONTROLLER_STACKSIZE      (THREAD_STACKSIZE_DEFAULT)

static void *_control_thread(void* arg);
static char _stack[CONTROLLER_STACKSIZE];

static controller_t _control;

static void _handle_input_event(event_t *event);

typedef struct {
    ts_event_t super;
    widget_t *widget;
    controller_action_widget_t action;
} controller_widget_event_t;

static controller_widget_event_t ev_widget = {
    .super = { .super = { .handler = _handle_input_event } }
};

static void _handle_input_event(event_t *event)
{
    controller_widget_event_t *ev = (controller_widget_event_t*)event;
    switch(ev->action) {
        case CONTROLLER_ACTION_WIDGET_LEAVE:
            gui_event_submit_switch_widget(widget_get_menu());
            break;
        case CONTROLLER_ACTION_WIDGET_HOME:
            gui_event_submit_switch_widget(widget_get_home());
            break;
        default:
            break;
    }
    ts_event_clear(&ev->super);
}

int controller_action_submit_input_action(widget_t *widget, controller_action_widget_t action)
{
    if (ts_event_claim(&ev_widget.super) == -EBUSY) {
        return -EBUSY;
    }
    LOG_DEBUG("[controller] Submitting widget action\n");
    ev_widget.widget = widget;
    ev_widget.action = action;
    event_post(&_control.queue, &ev_widget.super.super);
    return 0;
}

void controller_add_control_handler(controller_t *controller, control_event_handler_t *handler)
{
    /* See note above for reasons against clist.h */
    control_event_handler_t **last = &controller->handlers;
    handler->next = NULL;
    while (*last) {
        last = &(*last)->next;
    }
    *last = handler;
}

static void _submit_events(controller_t *controller, controller_event_t event)
{
    for (control_event_handler_t *handler = controller->handlers;
         handler; handler = handler->next) {
        if (handler->events & CONTROLLER_EVENT_FLAG(event)) {
            LOG_DEBUG("[controller]: Submitting event %u to %s\n", (unsigned)event,
                     handler->widget->spec->name);
            handler->widget->spec->event(handler->widget, event);
        }
    }
}

controller_t *controller_get(void)
{
	return &_control;
}

int controller_thread_create(void)
{
    int res = thread_create(_stack, CONTROLLER_STACKSIZE, CONTROLLER_THREAD_PRIO,
                            THREAD_CREATE_STACKTEST, _control_thread,
                            &_control, CONTROLLER_THREAD_NAME);
    return res;
}

static void *_control_thread(void* arg)
{
    controller_t *controller = arg;
    controller->pid = thread_getpid();
    event_queue_init(&_control.queue);
	controller_time_init();

    widget_init_installed();

    gui_event_submit_switch_widget(widget_get_home());
    while(1)
    {
        thread_flags_t flags = thread_flags_wait_any(
            THREAD_FLAG_EVENT | CONTROLLER_THREAD_FLAG_TICK
            );
        /* Tick event from the RTC */
        if (flags & CONTROLLER_THREAD_FLAG_TICK) {
            controller_update_time(controller);
            _submit_events(controller, CONTROLLER_EVENT_TICK);
        }
        if (flags & THREAD_FLAG_EVENT) {
            event_t *event = event_get(&controller->queue);
            if (event) {
                event->handler(event);
            }
        }
    }
    assert(false);
    /* should be never reached */
    return NULL;
}
