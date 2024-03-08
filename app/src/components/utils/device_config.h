#ifndef FIRMWARE_FLASH_STORAGE_H
#define FIRMWARE_FLASH_STORAGE_H

#include <stdint.h>
#include <stdbool.h>

#define STORAGE_FORMAT_VERSION 1

typedef struct device_config_t {
    uint8_t storage_fromat_version;
    uint8_t data_channel;
    uint8_t station_number;
    char station_alias[80];
    uint32_t crc32;
} device_config_t;

void flash_storage_init();
void flash_load_config();
void flash_save_config();

#endif //FIRMWARE_FLASH_STORAGE_H
