#include "spi_scraper.h"
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/printk.h>
#include <zephyr/kernel.h>
#include "punch_processing.h"

LOG_MODULE_REGISTER(spi_scraper, LOG_LEVEL_DBG);

#define SNIFF_CLCK_NODE DT_NODELABEL(clck0)
#define SNIFF_RESET_NODE DT_NODELABEL(reset0)
#define SNIFF_CS_NODE DT_NODELABEL(cs0)
#define SNIFF_MOSI_NODE DT_NODELABEL(mosi0)

static struct gpio_dt_spec sniff_clck_gpio = GPIO_DT_SPEC_GET(SNIFF_CLCK_NODE, gpios);
static struct gpio_dt_spec sniff_reset_gpio = GPIO_DT_SPEC_GET(SNIFF_RESET_NODE, gpios);
static struct gpio_dt_spec sniff_cs_gpio = GPIO_DT_SPEC_GET(SNIFF_CS_NODE, gpios);
static struct gpio_dt_spec sniff_mosi_gpio = GPIO_DT_SPEC_GET(SNIFF_MOSI_NODE, gpios);

static struct gpio_callback sniff_reset_cb;
static struct gpio_callback sniff_cs_cb;

uint8_t pattern[PATTERN_SIZE] = PATTERN; // Pattern for detecting a punch in the SPI communication
uint8_t scrape_buffer[PUNCH_BUFFER_SIZE] = {0};
bool scrape_enabled = false;
bool buffer_overflow = false;
static uint8_t scrape_buffer_bit_index = 0;

void spi_reset_change_cb(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    int ret;

    ret = gpio_pin_get_dt(&sniff_reset_gpio);
    if (ret < 0) {
        LOG_ERR("Error reading reset pin: %d", ret);
        return;
    }

    if (ret > 0) {
        scrape_enabled = true;
    } else {
        scrape_enabled = false;
    }
}

void spi_cs_triggered_cb(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    if (scrape_buffer_bit_index == PUNCH_BUFFER_SIZE * 8 &&
        memcmp(scrape_buffer, pattern, PATTERN_SIZE) == 0) {
        process_punch(scrape_buffer);
        LOG_INF("Punch detected.\n");
        // print out whole buffer
//        char buffer_str[PUNCH_BUFFER_SIZE * 3 + 1]; // 3 characters per byte (2 for hex and 1 for space) and 1 for null terminator
//        for (int i = 0; i < PUNCH_BUFFER_SIZE; i++) {
//            sprintf(&buffer_str[i * 3], "%02x ", scrape_buffer[i]);
//        }
//        printk("%s----\n", buffer_str);
    }

    scrape_buffer_bit_index = 0;
    buffer_overflow = false;

    memset(scrape_buffer, 0, sizeof(scrape_buffer));
}

static void initialize_inputs()
{
    int ret;

    // Check if devices are ready
    if (!device_is_ready(sniff_clck_gpio.port)) {
        LOG_ERR("Clock pin not ready.");
        return;
    }
    if (!device_is_ready(sniff_reset_gpio.port)) {
        LOG_ERR("Reset pin not ready.");
        return;
    }
    if (!device_is_ready(sniff_cs_gpio.port)) {
        LOG_ERR("Chip select pin not ready.");
        return;
    }
    if (!device_is_ready(sniff_mosi_gpio.port)) {
        LOG_ERR("MOSI pin not ready.");
        return;
    }

    ret = gpio_pin_configure_dt(&sniff_clck_gpio, GPIO_INPUT);
    if (ret < 0) {
        LOG_ERR("Error configuring clock pin: %d", ret);
        return;
    }
    ret = gpio_pin_configure_dt(&sniff_reset_gpio, GPIO_INPUT);
    if (ret < 0) {
        LOG_ERR("Error configuring reset pin: %d", ret);
        return;
    }
    ret = gpio_pin_configure_dt(&sniff_cs_gpio, GPIO_INPUT);
    if (ret < 0) {
        LOG_ERR("Error configuring chip select pin: %d", ret);
        return;
    }
    ret = gpio_pin_configure_dt(&sniff_mosi_gpio, GPIO_INPUT | GPIO_PULL_UP);
    if (ret < 0) {
        LOG_ERR("Error configuring MOSI pin: %d", ret);
        return;
    }
}

static void init_interrupts()
{
    int ret;

    ret = gpio_pin_interrupt_configure_dt(&sniff_reset_gpio, GPIO_INT_EDGE_BOTH);
    if (ret < 0) {
        LOG_ERR("Error configuring reset pin interrupt: %d", ret);
        return;
    }

    ret = gpio_pin_interrupt_configure_dt(&sniff_cs_gpio, GPIO_INT_EDGE_RISING);
    if (ret < 0) {
        LOG_ERR("Error configuring chip select pin interrupt: %d", ret);
        return;
    }
}

static void init_callbacks()
{
    int ret;

    gpio_init_callback(&sniff_reset_cb, spi_reset_change_cb, BIT(sniff_reset_gpio.pin));
    ret = gpio_add_callback(sniff_reset_gpio.port, &sniff_reset_cb);
    if (ret < 0) {
        LOG_ERR("Error adding callback for reset pin: %d", ret);
        return;
    }
    gpio_init_callback(&sniff_cs_cb, spi_cs_triggered_cb, BIT(sniff_cs_gpio.pin));
    ret = gpio_add_callback(sniff_cs_gpio.port, &sniff_cs_cb);
    if (ret < 0) {
        LOG_ERR("Error adding callback for reset pin: %d", ret);
        return;
    }
}

void spi_scraper_init()
{
    initialize_inputs();
    init_callbacks();
    init_interrupts();

    LOG_INF("Interrupts initialized.\n");
}


// Polling has been used instead of interrupts because of its superior performance with 125kHz SPI clock signal
// -- may be subject to change in the future, but for now it gets the job done good enough
void spi_sniffer(void *arg1, void *arg2, void *arg3)
{
    int last_clck_state = 0;
    int last_debug_state = 0;
    while (1) {
        if (!scrape_enabled || buffer_overflow) {
            k_sleep(K_USEC(10));
            continue;
        }

        int clck_state = gpio_pin_get_dt(&sniff_clck_gpio);
        if (clck_state < 0) {
            LOG_ERR("Error reading CLCK pin: %d", clck_state);
            continue;
        }

        if (clck_state == 0 && last_clck_state == 1) { // CLCK pin changed from high to low
            int mosi_state = gpio_pin_get_dt(&sniff_mosi_gpio);
            if (mosi_state < 0) {
                LOG_ERR("Error reading MOSI pin: %d", mosi_state);
                continue;
            }

            uint8_t bit_position = scrape_buffer_bit_index & 7; // equivalent to % 8
            uint8_t byte_position = scrape_buffer_bit_index >> 3; // equivalent to / 8

            if (mosi_state > 0) {
                scrape_buffer[byte_position] |= (1 << (7 - bit_position));
            } else {
                scrape_buffer[byte_position] &= ~(1 << (7 - bit_position));
            }

            scrape_buffer_bit_index++;
            if (scrape_buffer_bit_index >= (PUNCH_BUFFER_SIZE << 3)) { // equivalent to * 8
                buffer_overflow = true;
            }
        }

        last_clck_state = clck_state;
    }
}

K_THREAD_DEFINE(spi_sniffer_task, 1024, spi_sniffer, NULL, NULL, NULL, 7, 0, 0);