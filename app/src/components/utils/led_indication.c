#include "led_indication.h"
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <hal/nrf_gpio.h>

LOG_MODULE_REGISTER(led_indication, LOG_LEVEL_DBG);

//#define LED0_NODE DT_ALIAS(led0)
//#define LED1_NODE DT_ALIAS(led1)
//
//static const struct gpio_dt_spec led0_gpio = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
//static const struct gpio_dt_spec led1_gpio = GPIO_DT_SPEC_GET(LED1_NODE, gpios);

#define LED_1 NRF_GPIO_PIN_MAP(0,9)
#define LED_2 NRF_GPIO_PIN_MAP(0,10)
#define LED_3 NRF_GPIO_PIN_MAP(0,4)

void led_indication_init() {
    int ret;

//    if (!device_is_ready(led0_gpio.port)) {
//        return;
//    }
//    if(!device_is_ready(led1_gpio.port)) {
//        return;
//    }
//
//
//    ret = gpio_pin_configure_dt(&led0_gpio, GPIO_ACTIVE_LOW);
//    if (ret < 0) {
//        return;
//    }
//    ret = gpio_pin_configure_dt(&led1_gpio, GPIO_ACTIVE_LOW);
//    if (ret < 0) {
//        return;
//    }

    nrf_gpio_cfg_output(LED_1);
    nrf_gpio_cfg_output(LED_2);
    nrf_gpio_cfg_output(LED_3);

    LOG_INF("LED indication initialized.\n");
}

void led_blink() {
    int ret;
    LOG_INF("Blinking LED...\n");
//    ret = gpio_pin_toggle(led0_gpio.port, led0_gpio.pin);
//    if (ret < 0) {
//        return;
//    }
//    k_sleep(K_MSEC(300));
//    ret = gpio_pin_toggle(led0_gpio.port, led0_gpio.pin);
//    if (ret < 0) {
//        return;
//    }
    nrf_gpio_pin_toggle(LED_1);
    nrf_gpio_pin_toggle(LED_2);
    nrf_gpio_pin_toggle(LED_3);
}