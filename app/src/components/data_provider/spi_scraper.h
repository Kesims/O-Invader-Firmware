#ifndef FIRMWARE_SPI_SCRAPER_H
#define FIRMWARE_SPI_SCRAPER_H

#define PATTERN {0x84, 0x00, 0x00}
#define PATTERN_SIZE 3
void spi_scraper_init();

#endif //FIRMWARE_SPI_SCRAPER_H
