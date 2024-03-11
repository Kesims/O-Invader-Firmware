#include "configuration_service.h"
#include "utils/device_config.h"
#include "utils/device_sleep.h"
#include "utils/led_indication.h"

#include <zephyr/logging/log.h>
#include <zephyr/sys/reboot.h>


LOG_MODULE_REGISTER(configuration_service, LOG_LEVEL_DBG);

extern device_config_t device_config;

static ssize_t byte_read_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
    const uint8_t *value = attr->user_data;
    return bt_gatt_attr_read(conn, attr, buf, len, offset, value, sizeof(*value));
}

static ssize_t byte_write_callback(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    uint8_t *value = attr->user_data;
    if (offset + len > sizeof(*value)) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }
    memcpy(value + offset, buf, len);
    flash_save_config();
    return len;
}

static ssize_t char80_read_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
    const char *value = attr->user_data;
    return bt_gatt_attr_read(conn, attr, buf, len, offset, value, 80);
}

static ssize_t char80_write_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    char *value = attr->user_data;
    if (offset + len > 80) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }
    memcpy(value + offset, buf, len);
    flash_save_config();
    return len;
}

static ssize_t write_ident_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    ident_device();
    return len;
}

static ssize_t write_restart_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    sys_reboot(SYS_REBOOT_COLD);
}

static ssize_t write_sleep_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    device_sleep();
    return len;
}

// Declare the configuration service
BT_GATT_SERVICE_DEFINE(
        configuration_service,
        BT_GATT_PRIMARY_SERVICE(BT_UUID_CONF_SERVICE),
        BT_GATT_CHARACTERISTIC(
                BT_UUID_DATA_CHANNEL, // UUID
                BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, // Properties
                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, // Permissions
                byte_read_cb, // Read callback
                byte_write_callback, // Write callback
                &device_config.data_channel // User data
        ),
        BT_GATT_CHARACTERISTIC(
                BT_UUID_STATION_NUMBER, // UUID
                BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, // Properties
                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, // Permissions
                byte_read_cb, // Read callback
                byte_write_callback, // Write callback
                &device_config.station_number // User data
        ),
        BT_GATT_CHARACTERISTIC(
                BT_UUID_STATION_ALIAS, // UUID
                BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, // Properties
                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, // Permissions
                char80_read_cb, // Read callback
                char80_write_cb, // Write callback
                &device_config.station_alias // User data
        ),
        BT_GATT_CHARACTERISTIC(
                BT_UUID_IDENT, // UUID
                BT_GATT_CHRC_WRITE, // Properties
                BT_GATT_PERM_WRITE, // Permissions
                NULL, // Read callback
                write_ident_cb, // Write callback
                NULL // User data
        ),
        BT_GATT_CHARACTERISTIC(
                BT_UUID_RESTART, // UUID
                BT_GATT_CHRC_WRITE, // Properties
                BT_GATT_PERM_WRITE, // Permissions
                NULL, // Read callback
                write_restart_cb, // Write callback
                NULL // User data
        ),
        BT_GATT_CHARACTERISTIC(
                BT_UUID_SLEEP, // UUID
                BT_GATT_CHRC_WRITE, // Properties
                BT_GATT_PERM_WRITE, // Permissions
                NULL, // Read callback
                write_sleep_cb, // Write callback
                NULL // User data
        ),
);