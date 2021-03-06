# Rust on RIOT for PineTime Smart Watch

![Rust on RIOT on PineTime Smart Watch](https://lupyuen.github.io/images/rust-on-riot-small.jpg)

Rust on RIOT for [PineTime Smart Watch](https://wiki.pine64.org/index.php/PineTime) with [LVGL] and [NimBLE]. See the presentation and article...

1. [_Safer, Simpler Embedded Programs with Rust on RIOT_](https://docs.google.com/presentation/d/1IgCsWJ5TYpPaHXZlaETlM2rYQrBmOpN2WeFsNjmYO_k/edit?usp=sharing)

1. [_Video Presentation_](https://youtu.be/rTxeXnlH-mM)

1. [_Porting PineTime Watch Face from C to Rust On RIOT with LVGL_](https://lupyuen.github.io/pinetime-rust-riot/articles/watch_face)

1. [_WebAssembly Simulator for Rust on RIOT_](https://github.com/AppKaki/lvgl-wasm/blob/rust/README.md)

## Rust Watch Face

Rust source code for the watch face: [`rust/app/src/watch_face.rs`](rust/app/src/watch_face.rs)

Online WebAssembly Simulator demo: [_WebAssembly Simulator for Rust on RIOT_](https://github.com/AppKaki/lvgl-wasm/blob/rust/README.md)

## Download the Binaries

Download the binaries from [`Releases`](https://github.com/lupyuen/pinetime-rust-riot/releases)

## Build in the Cloud

The firmware is __built automatically__ in the GitHub Cloud. 

Just fork this repo in GitHub, click `Actions` to enable GitHub Actions, update the source code and the built firmware will be available for download under `Actions`.

The Automated Build is performed according to the following __GitHub Actions Workflow__...

[`.github/workflows/main.yml`](.github/workflows/main.yml)

The Worklow is similar to the one described in this article...

[_Build PineTime Firmware in the Cloud with GitHub Actions_](https://lupyuen.github.io/pinetime-rust-mynewt/articles/cloud)

## WebAssembly Simulator

We have a WebAssembly Simulator that will allow us to preview Rust on RIOT Watch Faces in a web browser.

[Online Demo of WebAssembly Simulator](https://lupyuen.github.io/pinetime-rust-riot/lvgl.html)

To set up the WebAssembly Simulator for Rust on RIOT on your fork, enable __GitHub Pages__ publishing for `master` branch, `docs` folder.

The WebAssembly Simulator for Rust on RIOT will be accessible at...

```
https://YOUR_GITHUB_ACCOUNT.github.io/pinetime-rust-riot
```

Replace `YOUR_GITHUB_ACCOUNT` by your GitHub Account name.

The WebAssembly Simulator is built according to the following __GitHub Actions Workflow__...

[`.github/workflows/simulate.yml`](.github/workflows/simulate.yml)

Note that the Simulator build will update files in the `docs` folder of your fork. Before committing any files, please pull updates from your fork first.

[More about WebAssembly Simulator for Rust on RIOT](https://github.com/AppKaki/lvgl-wasm/blob/rust/README.md)

The RIOT Firmware for PineTime Smart Watch is based on [`github.com/bosmoment/PineTime-apps`](https://github.com/bosmoment/PineTime-apps)...

![RIOT on PineTime Smart Watch](https://lupyuen.github.io/images/pinetime-riot.jpg)

# Bosmoment-style Pinetime firmware applications

Friendly firmware applications for the Pinetime!

This repository contains a number of applications and helper modules specific
for the [PineTime] open source smartwatch. 

Highlights:

- Built on top of [RIOT], an open source embedded operating system
- Apache [NimBLE] as an open-source bluetooth 5.0 stack.
- [LVGL] for the user interface.

## Features

- *Bluetooth LE*: provided by Nimble
- *Bluetooth pairing support*
- *Time synchronisation*: Bluetooth GATT Current Time Service client
- *Time keeping*: nRF52832 RTC for ticks per second time/date updates
- *Graphical user interface*: via LVGL.
- *Touch screen driver*: a cst816s touch screen driver with gesture support is included in the RIOT fork

And the default watch face screen includes:
- *Time/date*: Both time and date are shown on the main watch face with the
               time taking up the majority of the space.
- *Battery status*: Battery status is available on the main screen
- *Bluetooth status*: Bluetooth connection and advertisement mode is shown on
                      the screen

Multiple watch faces are supported with smooth scrolling between the different
faces. For now two dummy watch faces, one for notifications and one for
activity, are included as proof of concept.

## Structure

The project is separated into a number of directories:

- *RIOT*: The RIOT submodule used for compilation
- *apps*: Contains firmware applications for the PineTime.
- *modules*: Additional modules written to support the firmware applications
- *widgets*: A collection of small user applications or watch faces for the end
  user of the PineTime, for example: a stopwatch widget; a configuration
  widget; a heartbeat graph widget, a notification watch face.

## Getting started

### Repository setup

When checking out this repository, don't forget to initialize the RIOT submodule
contained within this repository with:

```Shellsession
git submodule init
git submodule update
```

### Building applications

Applications are contained in the `apps` dir with a single application per
directory. The `pinetime` application should give you a good starting point.

Use `make all` in the application directory to build the firmware, `make flash`
to flash it on the target and `make term` to get a serial connection to the
device. 

### Development

As the project is based on RIOT, it helps to get familiar with RIOT and check
which functionality is provided by the OS.
There is a [quick start] guide available for RIOT to get familiar with RIOT's
build system and to validate your toolchain functionality. Doxygen-based API
documentation is also available there.

Currently the Segger J-Link tools are used for flashing the application. It is
possible to use a different programmer by overriding the settings in the
`Makefile.include` for the `pinetime` board in the RIOT tree. 

## Bluetooth LE

Currently the firmware is always advertising if no connection is active. As
soon as a host connects to the PineTime, the advertising is stopped and
continues when the host disconnects.

Bonding is available and useable, but the bonds are not persistent between
reboots. (see Planned features).

A custom UUID is included in the advertisement to recognise the device by:
```
9851dc0a-b04a-1399-5646-3b38788cb1c5
```

This UUID can be used for Bluetooth LE filters in Android and such. The device
name itself can be customized to your liking and is not used for identifying the
device.

### Companion phone app

A fork of [GadgetBridge] is available with support for this firmware. The fork
uses the custom UUID specified above to filter and identify the device.

The GadgetBridge app should pick up the device when scanning and allow pairing
with the device. As long as the PineTime remembers the bond, the app and the
Pinetime should be able to restore the secure connection.

The GadgetBridge fork is configured to only allow encrypted reads to the current
time characteristic. The PineTime firmware automatically requests pairing when 
it receives an insufficient encryption error back from the app when requesting
the time.

## Tips

- STDIO is implemented via the Segger RTT protocol. It is non-blocking by
  default, blocking mode can be enabled by adding
  `STDIO_RTT_ENABLE_BLOCKING_STDOUT` to the CFLAGS. **Note**: the firmware will
  block (hang) on STDIO if no RTT client is attached.

- The directory where a RIOT tree is expected can be overridden by setting the
  `RIOTBASE` variable in the application makefile or from the command line.

- A fork of the Android [GadgetBridge] application is available with basic
  support for this RIOT PineTime application.

- By default the watchdog timer is running with a 5 second timeout. Every second
  the controller will kick the watchdog timer to reset the timeout, unless the
  button is pressed. A long press of the button will effectively cause a
  watchdog timer reset of the system.

## Planned features

- Device side Bluetooth key code confirmation (Only the host verifies the
  pairing code at the moment, the firmware always confirms the code as valid)
- Persistent Bluetooth bonds
- Secure over the air updates

[PineTIme]: https://www.pine64.org/pinetime/
[RIOT]: https://github.com/RIOT-os/RIOT/
[NimBLE]: https://github.com/apache/mynewt-nimble
[LVGL]: https://docs.lvgl.io/latest/en/html/index.html
[quick start]: https://doc.riot-os.org/index.html#the-quickest-start
[GadgetBridge]: https://codeberg.org/bergzand/Gadgetbridge/src/branch/riotwatch/initial
