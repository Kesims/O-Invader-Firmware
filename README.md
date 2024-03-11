# Firmware for the OriLive O-Invader board.

The board is designed to modify the existing capabilities of SportIdent BSF-9 by adding Bluetooth and wireless charging capability. It hooks directly onto the BSF-9 Flash <-> MCU communication by soldering a custom made FPC wire under the Flash module and replaces the device battery.

## Install dependencies
Install dependencies by running `west update` in the project directory. You need to set up west in advance ([west installation](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/zephyr/develop/west/install.html)).

## Building

Build the firmware for debug using `west build -b o-invader -- -DBOARD_ROOT="."` in the `app` directory.

Build the firmware for production using `west build -b o-invader -- -DBOARD_ROOT="." -DEXTRA_CONF_FILE="prj-prod.conf"`.

## Flashing
Flash the device using `west flash` command. The device can be than debugged using J-Link via RTT.


## Bluetooth LE endpoints

The device exposes the following standardized Bluetooth LE services:

- **Battery Service** - standard battery service `0x180F` with a single characteristic `0x2A19` for battery level.
- **Device Information Service** - standard device information service `0x180A` with characteristics `0x2A29` for manufacturer name, `0x2A24` for model number, `0x2A27` for hardware revision and `0x2A26` for firmware revision.

### Custom OriLive services:

- **Punch Information Service** - `e1e23000-fee9-4d67-a4f5-b5144cea14bd` with following characteristcs:
  - **Next Punch Characteristic** - `e1e23001-fee9-4d67-a4f5-b5144cea14bd` - write only, 1 byte. When written, the device will move the next available punch from internal queue to the endpoints described bellow.
  - **Available Punches Characteristic** - `e1e23002-fee9-4d67-a4f5-b5144cea14bd` - read + notify, 1 byte. Returns a number of available punches in the internal queue. When the number of available punches changes, the device will send a notification with the new value.
  - **Storage Overflow Characteristic** - `e1e23003-fee9-4d67-a4f5-b5144cea14bd` - read + notify, 1 byte. Returns `1` if the internal punch queue is full and the device is unable to store new punches, otherwise returns `0`.
  - **Punch SI Number** - `e1e23004-fee9-4d67-a4f5-b5144cea14bd` - read + notify, 4 bytes. Returns the SI number of the punch (Little-Endian). Sends notification when the data is ready after writing `Next Punch Characteristic`.
  - **Punch Timestamp** - `e1e23005-fee9-4d67-a4f5-b5144cea14bd` - read only. Returns the timestamp of the punch in ISO 8601 format.

    
- **Configuration Service** - `0xe1e22000-fee9-4d67-a4f5-b5144cea14bd` with following characteristics:
  - **Data Channel Characteristic** - `0xe1e22001-fee9-4d67-a4f5-b5144cea14bd` - read + write, 1 byte. 
  - **Station Number Characteristic** - `0xe1e22002-fee9-4d67-a4f5-b5144cea14bd` - read + write, 1 byte.
  - **Station Alias Characteristic** - `0xe1e22003-fee9-4d67-a4f5-b5144cea14bd` - read + write, max 80 bytes.
  - **Identify Characteristic** - `0xe1e22004-fee9-4d67-a4f5-b5144cea14bd` - write only, 1 byte. When written, the device will blink green LED for 25 seconds.
  - **Restart Characteristic** - `0xe1e22005-fee9-4d67-a4f5-b5144cea14bd` - write only, 1 byte. When written, the device will restart.
  - **Sleep Characteristic** - `0xe1e22006-fee9-4d67-a4f5-b5144cea14bd` - write only, 1 byte. When written, the device will enter sleep mode.


## Power management
O-Invader units contain a 500 mAh battery and can be charged using QI compliant wireless chargers.

The device can be in multiple states, but the main 3 states are:
- **Sleep** - consumption of 26 uA, BLE advertisements off. The device can be woken up by an SI punch. The punch sniffer is still fully functional. The device will enter sleep mode automatically after an hour of inactivity, or when the `Sleep Characteristic` is written.
- **Idle** - consumption of 64 uA, BLE advertisements on.
- **Connected** - consumption of 198 uA, with BLE client connected.
- **Active** - consumption varies, with the punch sniffer active and the device processing punches and powering the BSF-9 buzzer etc. Usually short large peaks.