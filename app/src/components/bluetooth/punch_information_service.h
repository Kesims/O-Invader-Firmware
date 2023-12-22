#ifndef FIRMWARE_PUNCH_INFORMATION_SERVICE_H
#define FIRMWARE_PUNCH_INFORMATION_SERVICE_H

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#define BT_UUID_PUNCH_INFORMATION_SERVICE_VAL \
	BT_UUID_128_ENCODE(0xe1e23000, 0xfee9, 0x4d67, 0xa4f5, 0xb5144cea14bd)

#define BT_UUID_PUNCH_DATA_VAL \
	BT_UUID_128_ENCODE(0xe1e23001, 0xfee9, 0x4d67, 0xa4f5, 0xb5144cea14bd)


#define BT_UUID_PUNCH_INFORMATION_SERVICE          BT_UUID_DECLARE_128(BT_UUID_PUNCH_INFORMATION_SERVICE_VAL)
#define BT_UUID_PUNCH_DATA                         BT_UUID_DECLARE_128(BT_UUID_PUNCH_DATA_VAL)


void punch_data_notify(uint64_t value);
void update_punch_data(uint64_t new_value);

#endif //FIRMWARE_PUNCH_INFORMATION_SERVICE_H
