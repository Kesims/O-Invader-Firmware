#include "led_indication.h"
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>

LOG_MODULE_REGISTER(led_indication, LOG_LEVEL_DBG);

#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)

static const struct gpio_dt_spec led0_gpio = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led1_gpio = GPIO_DT_SPEC_GET(LED1_NODE, gpios);

struct k_work device_ident_work;
void ident_device_work_handler(struct k_work *work);

void led_indication_init() {
    int ret;

    if (!device_is_ready(led0_gpio.port)) {
        return;
    }
    if(!device_is_ready(led1_gpio.port)) {
        return;
    }


    ret = gpio_pin_configure_dt(&led0_gpio, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        return;
    }
    ret = gpio_pin_configure_dt(&led1_gpio, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        return;
    }

    k_work_init(&device_ident_work, ident_device_work_handler);
    LOG_INF("LED indication initialized.\n");
}

void green_toggle() {
    gpio_pin_toggle(led1_gpio.port, led1_gpio.pin);
}

void red_toggle() {
    gpio_pin_toggle(led0_gpio.port, led0_gpio.pin);
}

void ident_device_work_handler(struct k_work *work) {
    for (int i = 0; i < 36; i++) {
        green_toggle();
        k_sleep(K_MSEC(200));
        green_toggle();
        k_sleep(K_MSEC(500));
    }
}

void ident_device() {
    k_work_submit(&device_ident_work);
}