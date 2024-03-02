#ifndef FIRMWARE_FLASH_STORAGE_H
#define FIRMWARE_FLASH_STORAGE_H

#include <stdint.h>
#include <stdbool.h>

#define STORAGE_FORMAT_VERSION 1

void flash_storage_init();
void flash_load_config();
void flash_save_config();

#endif //FIRMWARE_FLASH_STORAGE_H
