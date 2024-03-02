//#include "spi_scraper.h"
//#include <zephyr/drivers/spi.h>
//#include <zephyr/drivers/gpio.h>
//#include <zephyr/device.h>
//#include <zephyr/logging/log.h>
//#include <zephyr/devicetree.h>
//
//LOG_MODULE_REGISTER(spi_scraper, LOG_LEVEL_DBG);
//
//#define STACKSIZE 1024
//#define SPI_THREAD_PRIORITY 7
//#define FLAG_BYTE 0x84
//#define READ_LENGTH 12 // 1 flag byte + 11 data bytes
//
//
//void spi_scrape_data(void)
//{
//    const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(spi0));
//    if (!dev) {
//        printk("Could not find SPI device\n");
//        return;
//    }
//
//    struct spi_config spi_config = {
//            .frequency = 125000,
//            .operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(8),
//            .slave = 0,
//            .cs = NULL,
//    };
//
//    uint8_t rx_buffer[READ_LENGTH];
//    struct spi_buf rx_buf = {
//            .buf = rx_buffer,
//            .len = sizeof(rx_buffer),
//    };
//    struct spi_buf_set rx = {
//            .buffers = &rx_buf,
//            .count = 1,
//    };
//
//    while (1) {
//        k_yield();
//        int err = spi_transceive(dev, &spi_config, NULL, &rx);
//        if (err) {
//            printk("SPI transceive failed with error: %d\n", err);
//            continue;
//        }
//
//        if (rx_buffer[0] == FLAG_BYTE) {
//            printk("Flag byte received. Data: ");
//            for (int i = 1; i < READ_LENGTH; i++) {
//                printk("%02X ", rx_buffer[i]);
//            }
//            printk("\n");
//        }
//    }
//}
//
//void spi_thread(void)
//{
//    LOG_INF("SPI SNIFFER STARTED!\n");
//    spi_scrape_data();
//}
//
//#define SCK_PIN 5
//#define MOSI_PIN 9
//#define MISO_PIN 11
//
//void spi_sniffer_thread(void)
//{
//    const struct device *gpio_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0));
//    if (!gpio_dev) {
//        printk("Could not find GPIO device\n");
//        return;
//    }
//
//    gpio_pin_configure(gpio_dev, SCK_PIN, GPIO_INPUT);
//    gpio_pin_configure(gpio_dev, MOSI_PIN, GPIO_INPUT);
//    gpio_pin_configure(gpio_dev, MISO_PIN, GPIO_INPUT);
//
//    while (1) {
//        k_yield();
//
//        if (gpio_pin_get(gpio_dev, SCK_PIN) > 0) {
//            int mosi_bit = gpio_pin_get(gpio_dev, MOSI_PIN);
//            int miso_bit = gpio_pin_get(gpio_dev, MISO_PIN);
//
//            printk("SI: %d, SO: %d\n", mosi_bit, miso_bit);
//        }
//    }
//}
//
////K_THREAD_DEFINE(thread0_id, STACKSIZE, spi_sniffer_thread, NULL, NULL, NULL,
////SPI_THREAD_PRIORITY, 0, 0);
//
////K_THREAD_DEFINE(thread0_id, STACKSIZE, spi_thread, NULL, NULL, NULL,
////SPI_THREAD_PRIORITY, 0, 0);