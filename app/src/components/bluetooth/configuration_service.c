#include "configuration_service.h"

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(o_invader_configuration_service, LOG_LEVEL_DBG);

uint8_t data_channel = 0;

static ssize_t read_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
    const char *value = attr->user_data;
    return bt_gatt_attr_read(conn, attr, buf, len, offset, value, sizeof(*value));
}

static ssize_t write_callback (struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{

}


// Declare the configuration service
BT_GATT_SERVICE_DEFINE(
        configuration_service,
        BT_GATT_PRIMARY_SERVICE(BT_UUID_CONF_SERVICE),
        BT_GATT_CHARACTERISTIC(
                BT_UUID_DATA_CHANNEL, // UUID
                BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, // Properties
                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, // Permissions
                read_cb, // Read callback
                write_callback, // Write callback
                &data_channel // User data
        ),
);