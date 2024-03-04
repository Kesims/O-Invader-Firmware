#ifndef BASE_CONFIGURATION_SERVICE_H
#define BASE_CONFIGURATION_SERVICE_H

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>


#define BT_UUID_CONFIGURATION_SERVICE_VAL \
	BT_UUID_128_ENCODE(0xe1e22000, 0xfee9, 0x4d67, 0xa4f5, 0xb5144cea14bd)

#define BT_UUID_DATA_CHANNEL_VAL \
	BT_UUID_128_ENCODE(0xe1e22001, 0xfee9, 0x4d67, 0xa4f5, 0xb5144cea14bd)

#define BT_UUID_STATION_NUMBER_VAL \
    BT_UUID_128_ENCODE(0xe1e22002, 0xfee9, 0x4d67, 0xa4f5, 0xb5144cea14bd)

#define BT_UUID_STATION_ALIAS_VAL \
    BT_UUID_128_ENCODE(0xe1e22003, 0xfee9, 0x4d67, 0xa4f5, 0xb5144cea14bd)

#define BT_UUID_IDENT_VAL \
    BT_UUID_128_ENCODE(0xe1e22004, 0xfee9, 0x4d67, 0xa4f5, 0xb5144cea14bd)

#define BT_UUID_RESTART_VAL \
    BT_UUID_128_ENCODE(0xe1e22005, 0xfee9, 0x4d67, 0xa4f5, 0xb5144cea14bd)

#define BT_UUID_CONF_SERVICE                BT_UUID_DECLARE_128(BT_UUID_CONFIGURATION_SERVICE_VAL)
#define BT_UUID_DATA_CHANNEL                BT_UUID_DECLARE_128(BT_UUID_DATA_CHANNEL_VAL)
#define BT_UUID_STATION_NUMBER              BT_UUID_DECLARE_128(BT_UUID_STATION_NUMBER_VAL)
#define BT_UUID_STATION_ALIAS               BT_UUID_DECLARE_128(BT_UUID_STATION_ALIAS_VAL)
#define BT_UUID_IDENT                       BT_UUID_DECLARE_128(BT_UUID_IDENT_VAL)
#define BT_UUID_RESTART                     BT_UUID_DECLARE_128(BT_UUID_RESTART_VAL)


#endif //BASE_CONFIGURATION_SERVICE_H
