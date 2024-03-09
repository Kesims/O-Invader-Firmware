#ifndef BASE_BLUETOOTH_CORE_H
#define BASE_BLUETOOTH_CORE_H

// Includes
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gap.h>

// Defines
#define DEVICE_NAME CONFIG_BT_DEVICE_NAME            // Device name extracted from prj.conf
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

// Function prototypes
void bt_core_initialize();
void bt_start_advertising();
void bt_pause_advertising();
bool is_device_connected();

#endif //BASE_BLUETOOTH_CORE_H
