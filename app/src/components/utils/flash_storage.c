#include <zephyr/kernel.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <stdio.h>
#include <zephyr/logging/log.h>
#include "flash_storage.h"

LOG_MODULE_REGISTER(bp_flash_storage, LOG_LEVEL_DBG);

#define STORAGE_PARTITION	storage_partition

#define STORAGE_PARTITION_OFFSET	FIXED_PARTITION_OFFSET(STORAGE_PARTITION)
#define STORAGE_PARTITION_DEVICE	FIXED_PARTITION_DEVICE(STORAGE_PARTITION)
#define FLASH_PAGE_SIZE   4096

struct k_work flash_save_work;

const struct device *flash_dev;
struct flash_pages_info info;

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
}

//void flash_load_config() {
//    if (flash_read(flash_dev, STORAGE_PARTITION_OFFSET, &config, sizeof(config)) != 0) {
//        LOG_ERR("Error: Unable to read flash\n");
//        return;
//    }
//    if (config.storage_fromat_version != STORAGE_FORMAT_VERSION) {
//        LOG_ERR("Error: Invalid storage format version\n");
//        return;
//    }
//    stim_out_1 = unpack_boolean(config.stim_out, 0);
//    stim_out_2 = unpack_boolean(config.stim_out, 1);
//    stim_out_3 = unpack_boolean(config.stim_out, 2);
//    stim_out_4 = unpack_boolean(config.stim_out, 3);
//    stim_freq = config.stim_freq;
//    stim_freq_spread = config.stim_freq_spread;
//}
//
//void flash_save_config() {
//    // Handled in a workqueue, because invoking
//    // for example from BLE callback would be unsafe
//    k_work_submit(&flash_save_work);
//}
//
//static void flash_save_config_work(struct k_work *work) {
//    if (flash_erase(flash_dev, STORAGE_PARTITION_OFFSET, FLASH_PAGE_SIZE) != 0) {
//        LOG_ERR("Error: Unable to erase flash\n");
//        return;
//    }
//    if (flash_write(flash_dev, STORAGE_PARTITION_OFFSET, &config, sizeof(config)) != 0) {
//        LOG_ERR("Error: Unable to write flash\n");
//        return;
//    }
//}