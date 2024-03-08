#include "punch_information_service.h"
#include "../data_provider/punch_provider.h"
#include <zephyr/logging/log.h>


LOG_MODULE_REGISTER(punch_information_service, LOG_LEVEL_DBG);

extern punch_data_t current_punch;
extern bool storage_overflow;
extern uint16_t stored_punch_count;

static ssize_t byte_read_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
    const char *value = attr->user_data;
    return bt_gatt_attr_read(conn, attr, buf, len, offset, value, sizeof(*value));
}

static ssize_t uint32_read_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset, attr->user_data, sizeof(uint32_t));
}

static ssize_t char80_read_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset, attr->user_data, 20);
}

bool send_punches_available = false;
bool send_storage_overflow = false;
bool send_punch_si_number = false;

void cccd_changed_punches_cb(const struct bt_gatt_attr *attr, uint16_t value)
{
    send_punches_available = (value == BT_GATT_CCC_NOTIFY);
}

void cccd_changed_overflow_cb(const struct bt_gatt_attr *attr, uint16_t value)
{
    send_storage_overflow = (value == BT_GATT_CCC_NOTIFY);
}

void cccd_changed_si_number_cb(const struct bt_gatt_attr *attr, uint16_t value)
{
    send_punch_si_number = (value == BT_GATT_CCC_NOTIFY);
}

int next_punch_write(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    retrieve_next_punch();
    return len;
}

// Declare the configuration service
BT_GATT_SERVICE_DEFINE(
        punch_information_service,
        BT_GATT_PRIMARY_SERVICE(BT_UUID_PUNCH_INFORMATION_SERVICE),
        BT_GATT_CHARACTERISTIC(
                BT_UUID_GET_NEXT_PUNCH, // UUID
                BT_GATT_CHRC_WRITE, // Properties
                BT_GATT_PERM_WRITE, // Permissions
                NULL, // Read callback
                next_punch_write, // Write callback
                NULL // User data
        ),
        BT_GATT_CHARACTERISTIC(
                BT_UUID_GET_PUNCHES_AVAILABLE, // UUID
                BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY, // Properties
                BT_GATT_PERM_READ, // Permissions
                byte_read_cb, // Read callback
                NULL, // Write callback
                &stored_punch_count // User data
        ),
        BT_GATT_CCC(
                cccd_changed_punches_cb,
                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE
        ),
        BT_GATT_CHARACTERISTIC(
                BT_UUID_STORAGE_OVERFLOW, // UUID
                BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY, // Properties
                BT_GATT_PERM_READ, // Permissions
                byte_read_cb, // Read callback
                NULL, // Write callback
                &storage_overflow // User data
        ),
        BT_GATT_CCC(
                cccd_changed_overflow_cb,
                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE
        ),
        BT_GATT_CHARACTERISTIC(
            BT_UUID_PUNCH_SI_NUMBER, // UUID
            BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY, // Properties
            BT_GATT_PERM_READ, // Permissions
            uint32_read_cb, // Read callback
            NULL, // Write callback
            &current_punch.si_number // User data
        ),
        BT_GATT_CCC(
                cccd_changed_si_number_cb,
                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE
        ),
        BT_GATT_CHARACTERISTIC(
                BT_UUID_PUNCH_TIME, // UUID
                BT_GATT_CHRC_READ, // Properties
                BT_GATT_PERM_READ, // Permissions
                char80_read_cb, // Read callback
                NULL, // Write callback
                &current_punch.iso8601_time // User data
        ),
);


struct my_work {
    struct k_work work;
    uint8_t value;
};

struct my_work notify_work_data;

void punches_available_notify_work_handler(struct k_work *work)
{
    struct my_work *notify_work = CONTAINER_OF(work, struct my_work, work);
    uint8_t value = notify_work->value;
    if(!send_punches_available) return;
    int res = bt_gatt_notify(NULL, &punch_information_service.attrs[3], &value, sizeof(value));
    if(res) {
        LOG_ERR("Failed to notify, error code: %d", res);
    }
}

void punches_available_notify(uint8_t value)
{
    if(!send_punches_available) return;
    notify_work_data.value = value;
    k_work_init(&notify_work_data.work, punches_available_notify_work_handler);
    k_work_submit(&notify_work_data.work);
}

void storage_overflow_notify(uint8_t value)
{
    if(!send_storage_overflow) return;
    bt_gatt_notify(NULL, &punch_information_service.attrs[7], &value, sizeof(value));
}

void punch_si_number_notify(uint32_t value)
{
    if(!send_punch_si_number) return;
    bt_gatt_notify(NULL, &punch_information_service.attrs[9], &value, sizeof(value));
}

