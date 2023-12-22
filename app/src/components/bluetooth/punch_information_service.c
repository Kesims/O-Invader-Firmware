#include "punch_information_service.h"

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(punch_information_service, LOG_LEVEL_DBG);

uint64_t punch_data = 0;

static ssize_t read_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
    const char *value = attr->user_data;
    return bt_gatt_attr_read(conn, attr, buf, len, offset, value, sizeof(*value));
}

void cccd_changed_cb(const struct bt_gatt_attr *attr, uint16_t value)
{
    ARG_UNUSED(attr);
    switch(value)
    {
        case BT_GATT_CCC_NOTIFY:
            // Start sending stuff!
            break;

        case BT_GATT_CCC_INDICATE:
            // Start sending stuff via indications
            break;

        case 0:
            // Stop sending stuff
            break;

        default:
            printk("Error, CCCD has been set to an invalid value");
    }
}

// Declare the configuration service
BT_GATT_SERVICE_DEFINE(
        punch_information_service,
        BT_GATT_PRIMARY_SERVICE(BT_UUID_PUNCH_INFORMATION_SERVICE),
        BT_GATT_CHARACTERISTIC(
            BT_UUID_PUNCH_DATA, // UUID
            BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY, // Properties
            BT_GATT_PERM_READ, // Permissions
            read_cb, // Read callback
            NULL, // Write callback
            &punch_data // User data
        ),
        BT_GATT_CCC(
            cccd_changed_cb,
            BT_GATT_PERM_READ | BT_GATT_PERM_WRITE
        ),
);

void punch_data_notify(uint64_t value)
{
    bt_gatt_notify(NULL, &punch_information_service.attrs[1], &value, sizeof(value)); // Null conn param means notify all connections
}

void update_punch_data(uint64_t new_value)
{
    punch_data = new_value;
    punch_data_notify(new_value);
}