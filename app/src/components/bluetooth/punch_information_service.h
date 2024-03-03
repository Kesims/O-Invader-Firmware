#ifndef FIRMWARE_PUNCH_INFORMATION_SERVICE_H
#define FIRMWARE_PUNCH_INFORMATION_SERVICE_H

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#define BT_UUID_PUNCH_INFORMATION_SERVICE_VAL \
	BT_UUID_128_ENCODE(0xe1e23000, 0xfee9, 0x4d67, 0xa4f5, 0xb5144cea14bd)

#define BT_UUID_GET_NEXT_PUNCH_VAL \
	BT_UUID_128_ENCODE(0xe1e23001, 0xfee9, 0x4d67, 0xa4f5, 0xb5144cea14bd)

#define BT_UUID_GET_PUNCHES_AVAILABLE_VAL \
	BT_UUID_128_ENCODE(0xe1e23002, 0xfee9, 0x4d67, 0xa4f5, 0xb5144cea14bd)

#define BT_UUID_STORAGE_OVERFLOW_VAL \
	BT_UUID_128_ENCODE(0xe1e23003, 0xfee9, 0x4d67, 0xa4f5, 0xb5144cea14bd)

#define BT_UUID_PUNCH_SI_NUMBER_VAL \
    BT_UUID_128_ENCODE(0xe1e23004, 0xfee9, 0x4d67, 0xa4f5, 0xb5144cea14bd)

#define BT_UUID_PUNCH_TIME_VAL \
    BT_UUID_128_ENCODE(0xe1e23005, 0xfee9, 0x4d67, 0xa4f5, 0xb5144cea14bd)

#define BT_UUID_PUNCH_INFORMATION_SERVICE           BT_UUID_DECLARE_128(BT_UUID_PUNCH_INFORMATION_SERVICE_VAL)
#define BT_UUID_GET_NEXT_PUNCH                      BT_UUID_DECLARE_128(BT_UUID_GET_NEXT_PUNCH_VAL)
#define BT_UUID_GET_PUNCHES_AVAILABLE               BT_UUID_DECLARE_128(BT_UUID_GET_PUNCHES_AVAILABLE_VAL)
#define BT_UUID_STORAGE_OVERFLOW                    BT_UUID_DECLARE_128(BT_UUID_STORAGE_OVERFLOW_VAL)
#define BT_UUID_PUNCH_SI_NUMBER                     BT_UUID_DECLARE_128(BT_UUID_PUNCH_SI_NUMBER_VAL)
#define BT_UUID_PUNCH_TIME                          BT_UUID_DECLARE_128(BT_UUID_PUNCH_TIME_VAL)

void punches_available_notify(uint8_t value);
void storage_overflow_notify(uint8_t value);
void punch_si_number_notify(uint32_t value);

#endif //FIRMWARE_PUNCH_INFORMATION_SERVICE_H
