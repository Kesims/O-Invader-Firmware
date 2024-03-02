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

    LOG_INF("LED indication initialized.\n");
}

void led_blink() {
    int ret;
    LOG_INF("Blinking LED...\n");
    ret = gpio_pin_set(led1_gpio.port, led1_gpio.pin, 0);
    if (ret < 0) {
        return;
    }
    k_sleep(K_MSEC(100));
    ret = gpio_pin_set(led1_gpio.port, led1_gpio.pin, 1);
    if (ret < 0) {
        return;
    }
}

void red_led_toggle() {
    int ret;
    ret = gpio_pin_toggle(led0_gpio.port, led0_gpio.pin);
    if (ret < 0) {
        return;
    }
}