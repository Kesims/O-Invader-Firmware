#include <zephyr/kernel.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <stdio.h>
#include <zephyr/logging/log.h>
#include "device_config.h"
#include "crc32.h"

LOG_MODULE_REGISTER(bp_flash_storage, LOG_LEVEL_DBG);

#define STORAGE_PARTITION	storage_partition

#define STORAGE_PARTITION_OFFSET	FIXED_PARTITION_OFFSET(STORAGE_PARTITION)
#define STORAGE_PARTITION_DEVICE	FIXED_PARTITION_DEVICE(STORAGE_PARTITION)
#define FLASH_PAGE_SIZE   4096

struct k_work flash_save_work;
const struct device *flash_dev;
struct flash_pages_info info;

device_config_t device_config = {
    .storage_fromat_version = STORAGE_FORMAT_VERSION,
    .data_channel = 0,
    .station_number = 0,
    .station_alias = "",
    .crc32 = 0
};

static void flash_save_config_work(struct k_work *work);

void flash_storage_init() {
    flash_dev = STORAGE_PARTITION_DEVICE;
    if (flash_dev == NULL) {
        LOG_ERR("Error: No flash device found\n");
        return;
    }
    if (flash_get_page_info_by_offs(flash_dev, STORAGE_PARTITION_OFFSET, &info) != 0) {
        LOG_ERR("Error: Unable to get page info\n");
        return;
    }
    LOG_INF("Flash storage initialized.\n");
    k_work_init(&flash_save_work, flash_save_config_work);
    flash_load_config();
}

void reset_device_config() {
    memset(&device_config, 0, sizeof(device_config));
    device_config.storage_fromat_version = STORAGE_FORMAT_VERSION;
    LOG_INF("Generated new default device config\n");

}

void flash_load_config() {
    if (flash_read(flash_dev, STORAGE_PARTITION_OFFSET, &device_config, sizeof(device_config)) != 0) {
        LOG_ERR("Error: Unable to read flash\n");
        reset_device_config();
        return;
    }
    if (device_config.storage_fromat_version != STORAGE_FORMAT_VERSION) {
        LOG_ERR("Error: Invalid storage format version\n");
        reset_device_config();
        return;
    }
    // verify the CRC matches the data
    uint32_t crc = calculate_crc32((const uint8_t *) &device_config,
                                   sizeof(device_config) - sizeof(device_config.crc32));
    if (crc != device_config.crc32) {
        LOG_ERR("Error: Invalid CRC in device config loaded from flash\n");
        reset_device_config();
        return;
    }
}

void flash_save_config() {
    // Handled in a workqueue, because invoking
    // for example from BLE callback would be unsafe
    k_work_submit(&flash_save_work);
}

static void flash_save_config_work(struct k_work *work) {
    device_config.crc32 = calculate_crc32((const uint8_t *) &device_config,
                                          sizeof(device_config) - sizeof(device_config.crc32));

    if (flash_erase(flash_dev, STORAGE_PARTITION_OFFSET, FLASH_PAGE_SIZE) != 0) {
        LOG_ERR("Error: Unable to erase flash\n");
        return;
    }
    if (flash_write(flash_dev, STORAGE_PARTITION_OFFSET, &device_config, sizeof(device_config)) != 0) {
        LOG_ERR("Error: Unable to write flash\n");
        return;
    }
}