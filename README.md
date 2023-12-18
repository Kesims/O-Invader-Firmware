# Firmware for the OriLive O-Invader board.

The board is designed to modify the existing capabilities of SportIdent BSF-9 by adding Bluetooth and wireless charging capability. It hooks directly onto the BSF-9 Flash <-> MCU communication by soldering a custom made FPC wire under the Flash module and replaces the device battery.

## Install dependencies
Install dependencies by running `west update` in the project directory. You need to set up west in advance ([west installation](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/zephyr/develop/west/install.html)).

## Building

Build the firmware for debug using `west build -b o-invader -- -DBOARD_ROOT="."` in the `app` directory.

Build the firmware for production using `west build -b o-invader -- -DBOARD_ROOT="." -DEXTRA_CONF_FILE="prj-prod.conf"`.

## Flashing
Flash the device using `west flash` command. The device can be than debugged using J-Link via RTT.
