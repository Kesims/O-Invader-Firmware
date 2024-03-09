#ifndef FIRMWARE_SPI_SCRAPER_H
#define FIRMWARE_SPI_SCRAPER_H

#define PATTERN {0x84, 0x00, 0x00}
#define PATTERN_SIZE 3
void spi_scraper_init();
void spi_sniffer(void *arg1, void *arg2, void *arg3);

#endif //FIRMWARE_SPI_SCRAPER_H
