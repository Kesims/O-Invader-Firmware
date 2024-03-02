#include "spi_scraper.h"
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <zephyr/devicetree.h>
#include "../utils/led_indication.h"

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
static struct gpio_callback sniff_clck_cb;
static struct gpio_callback sniff_cs_cb;

uint8_t pattern[PATTERN_SIZE] = PATTERN; // Pattern for detecting a punch in the SPI communication
uint8_t scrape_buffer[SPI_SCRAPER_BUFFER_SIZE] = {0};
bool scrape_enabled = false;
bool buffer_overflow = false;
static uint8_t scrape_buffer_bit_index = 0;

void spi_clck_tick_cb(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    if (scrape_enabled) {
        if (buffer_overflow) {
            return;
        }

        int ret;

        ret = gpio_pin_get_dt(&sniff_mosi_gpio);
        if (ret < 0) {
            LOG_ERR("Error reading MOSI pin: %d", ret);
            return;
        }

        uint8_t bit_position = scrape_buffer_bit_index % 8;
        uint8_t byte_position = scrape_buffer_bit_index / 8;

        if (ret > 0) {
            scrape_buffer[byte_position] |= (1 << bit_position);
        } else {
            scrape_buffer[byte_position] &= ~(1 << bit_position);
        }

        scrape_buffer_bit_index++;
        if (scrape_buffer_bit_index >= SPI_SCRAPER_BUFFER_SIZE * 8) {
            buffer_overflow = true;
        }
    }
}

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
    if(scrape_buffer_bit_index == SPI_SCRAPER_BUFFER_SIZE * 8) {
        red_led_toggle();
    }
    if (scrape_buffer_bit_index == SPI_SCRAPER_BUFFER_SIZE * 8 &&
        memcmp(scrape_buffer, pattern, PATTERN_SIZE) == 0) {
//        processPunch(scrape_buffer);
        LOG_INF("Punch detected.\n");
    } else {
        memset(scrape_buffer, 0, sizeof(scrape_buffer));
        scrape_buffer_bit_index = 0;
        buffer_overflow = false;
    }
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

    ret = gpio_pin_interrupt_configure_dt(&sniff_reset_gpio, GPIO_INT_EDGE_RISING);
    if (ret < 0) {
        LOG_ERR("Error configuring reset pin interrupt: %d", ret);
        return;
    }

    ret = gpio_pin_interrupt_configure_dt(&sniff_clck_gpio, GPIO_INT_EDGE_RISING);
    if (ret < 0) {
        LOG_ERR("Error configuring clock pin interrupt: %d", ret);
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
    gpio_init_callback(&sniff_clck_cb, spi_clck_tick_cb, BIT(sniff_clck_gpio.pin));
    ret = gpio_add_callback(sniff_clck_gpio.port, &sniff_clck_cb);
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