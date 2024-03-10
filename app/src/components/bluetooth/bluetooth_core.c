#include "bluetooth_core.h"
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/addr.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/conn.h>

LOG_MODULE_REGISTER(bluetooth_core, LOG_LEVEL_DBG);


uint8_t COMPANY_ID[] = { 0x00, 0x00};


// Define the advertising data
static const struct bt_data ad[] = {
        // Set the advertising flags
        BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
        // Set the advertising packet data
        BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

static struct bt_le_adv_param *adv_param = BT_LE_ADV_PARAM((BT_LE_ADV_OPT_CONNECTABLE|BT_LE_ADV_OPT_USE_IDENTITY),
                                                           800, /*Min Advertising Interval 500ms (800*0.625ms) */
                                                           801, /*Max Advertising Interval 500.625ms (801*0.625ms)*/
                                                           NULL); /* Set to NULL for undirected advertising*/


// Define the scan response data
static const struct bt_data sd[] = {};

bool device_connected = false;

static void connected(struct bt_conn *conn, uint8_t err)
{
    if(err) {
        LOG_INF("Connection failed (err %u)\n", err);
        return;
    }
    device_connected = true;
    LOG_INF("Device connected\n");
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    device_connected = false;
    LOG_INF("Device disconnected (reason %u)\n", reason);
}

static struct bt_conn_cb conn_callbacks = {
        .connected = connected,
        .disconnected = disconnected,
};

void bt_pause_advertising() {
    int err;
    err = bt_le_adv_stop();
    if (err) {
        LOG_ERR("Advertising failed to stop (err %d)\n", err);
        return;
    }
    LOG_INF("Advertising successfully stopped\n");
}

void bt_start_advertising() {
    int err;
    err = bt_le_adv_start(adv_param, ad, ARRAY_SIZE(ad),
                          sd, ARRAY_SIZE(sd));
    if (err) {
        LOG_ERR("Advertising failed to start (err %d)\n", err);
        return;
    }
    LOG_INF("Advertising successfully started\n");
}


void bt_core_initialize() {
    int err;
    err = bt_enable(NULL);
    if (err) {
        LOG_ERR("Bluetooth init failed (err %d)\n", err);
        return;
    }
    LOG_INF("Bluetooth initialized\n");
    bt_conn_cb_register(&conn_callbacks);

    bt_start_advertising();
}

bool is_device_connected() {
    return device_connected;
}