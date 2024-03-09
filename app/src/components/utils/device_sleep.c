#include "device_sleep.h"
#include "../bluetooth/bluetooth_core.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(device_sleep, LOG_LEVEL_DBG);

void timer_tick();

K_TIMER_DEFINE(device_sleep_timer, timer_tick, NULL);

uint32_t inactivity_time = 0; // in seconds
bool device_sleeping = false;

void device_wake_up() {
    LOG_INF("Waking up device\n");
    device_sleeping = false;
    bt_start_advertising();
    k_timer_start(&device_sleep_timer, K_SECONDS(DEVICE_SLEEP_TIMER_STEP), K_SECONDS(DEVICE_SLEEP_TIMER_STEP));
}

void device_sleep() {
    LOG_INF("Putting device to sleep\n");
    device_sleeping = true;
    bt_pause_advertising();
    k_timer_stop(&device_sleep_timer);
}

struct k_work device_wake_up_work;
struct k_work device_sleep_work;

void device_wake_up_work_handler(struct k_work *work) {
    device_wake_up();
}

void device_sleep_work_handler(struct k_work *work) {
    device_sleep();
}

void device_activate() {
    // Reset the inactivity time
    inactivity_time = 0;
    if (device_sleeping) {
        k_work_submit(&device_wake_up_work);
    }
}

void timer_tick() {
    if (device_sleeping || is_device_connected()) {
        return;
    }
    inactivity_time += 10;
    if (inactivity_time > DEVICE_SLEEP_TIMEOUT) {
        k_work_submit(&device_sleep_work);
    }
    LOG_INF("Inactivity time: %d\n", inactivity_time);
}

void device_sleep_init() {

    k_work_init(&device_wake_up_work, device_wake_up_work_handler);
    k_work_init(&device_sleep_work, device_sleep_work_handler);

    // Start the timer to run every second
    k_timer_start(&device_sleep_timer, K_SECONDS(DEVICE_SLEEP_TIMER_STEP), K_SECONDS(DEVICE_SLEEP_TIMER_STEP));
}