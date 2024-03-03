#include "punch_processing.h"
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include "punch_provider.h"

LOG_MODULE_REGISTER(punch_processing, LOG_LEVEL_DBG);

// Data format:
// 4 bytes preamble
// 3 bytes SINumber
// 1 byte, bits 7..2: year (0 means 2000), bits 1..0: upper bits of month
// 1 byte, bits 7..6: lower bits of month, bits 5..1: day of month, bit 0: AM/PM
// 2 bytes, seconds since midnight or midday
// 1 byte, divide by 256 to get fractions of seconds

uint8_t punch_buffer[PUNCH_BUFFER_SIZE] = {0};

static K_THREAD_STACK_DEFINE(punch_stack_area, 512);
struct k_work punch_work;
struct k_work_q punch_work_queue;

// Retrieves the data from interrupt in spi_scraper, and submits it to the punch processing work queue
void process_punch(uint8_t *punch_data)
{
    memcpy(punch_buffer, punch_data, PUNCH_BUFFER_SIZE);
    k_work_submit_to_queue(&punch_work_queue, &punch_work);
}

void get_punch_values_from_data(uint8_t *punch_data)
{
    // Extract the SI number
    uint32_t si_number = (punch_data[4] << 16) | (punch_data[5] << 8) | punch_data[6];

    // Extract the year, date and time
    uint16_t year = (punch_data[7] >> 2) + 2000;
    uint8_t month = ((punch_data[7] & 0x03) << 2) | ((punch_data[8] >> 6) & 0x03);
    uint8_t day = (punch_data[8] >> 1) & 0x1F;
    bool is_pm = punch_data[8] & 0x01;
    uint16_t seconds = (punch_data[9] << 8) | punch_data[10];
    uint8_t fraction = punch_data[11];


    // Print the punch data
    LOG_INF("SI number: %u\n", si_number);
    LOG_INF("Date: %u-%u-%u\n", year, month, day);
    LOG_INF("Time: %u:%u:%u.%u\n", seconds / 3600 + (is_pm ? 12 : 0), (seconds / 60) % 60, seconds % 60, ((int)(fraction * 1000) / 256.0f));

    punch_data_t punch = {
            .si_number = si_number
    };
    char iso8601_time[20];
    sprintf(iso8601_time, "%u-%u-%uT%u:%u:%u.%u", year, month, day, seconds / 3600 + (is_pm ? 12 : 0), (seconds / 60) % 60, seconds % 60, (uint32_t)(fraction * 1000) / 256);
    memcpy(punch.iso8601_time, iso8601_time, 20);
    store_punch(punch);
}

void offload_function(struct k_work *work)
{
    get_punch_values_from_data(punch_buffer);
}

void initialize_punch_processing()
{
    k_work_queue_start(&punch_work_queue, punch_stack_area,
                       K_THREAD_STACK_SIZEOF(punch_stack_area), 4,
                       NULL);
    k_work_init(&punch_work, offload_function);
    LOG_INF("Punch processing work queue initialized.\n");
}


