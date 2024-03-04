#include "punch_provider.h"
#include "bluetooth/punch_information_service.h"
#include <zephyr/logging/log.h>


LOG_MODULE_REGISTER(punch_provider, LOG_LEVEL_DBG);


punch_data_t current_punch;
bool storage_overflow = false;
uint16_t stored_punch_count = 0;
punch_data_t punch_storage[PUNCH_STORAGE_SIZE];
int punch_storage_start = 0;
int punch_storage_end = 0;

void store_punch(punch_data_t new_punch) {
    // Check if the last punch si_number is the same
    if (stored_punch_count > 0 && new_punch.si_number == punch_storage[(punch_storage_end - 1 + PUNCH_STORAGE_SIZE) % PUNCH_STORAGE_SIZE].si_number) {
        return;
    }

    if ((punch_storage_end + 1) % PUNCH_STORAGE_SIZE == punch_storage_start && stored_punch_count > 0) {
        // Buffer is full
        storage_overflow = true;
        storage_overflow_notify(true);
        return;
    }

    // Add the new punch to the buffer
    punch_storage[punch_storage_end] = new_punch;
    stored_punch_count++;

    // Move the end pointer forward, wrapping around to the start if necessary
    punch_storage_end = (punch_storage_end + 1) % PUNCH_STORAGE_SIZE;
    punches_available_notify(stored_punch_count);

    LOG_INF("SI number: %u\n", new_punch.si_number);
    LOG_INF("Time: %s\n", new_punch.iso8601_time);
}

int retrieve_next_punch() {
    if (punch_storage_start == punch_storage_end && stored_punch_count == 0) {
        // Buffer is empty
        return -1;
    }

    // Copy the punch to current_punch
    current_punch = punch_storage[punch_storage_start];
    stored_punch_count--;

    if(storage_overflow) {
        storage_overflow = false;
        storage_overflow_notify(false);
    }

    // Move the start pointer forward, wrapping around to the start if necessary
    punch_storage_start = (punch_storage_start + 1) % PUNCH_STORAGE_SIZE;

    punch_si_number_notify(current_punch.si_number);
    punches_available_notify(stored_punch_count);
    return 0;
}