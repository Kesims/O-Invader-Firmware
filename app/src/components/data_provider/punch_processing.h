#ifndef BUGPACK_BASE_PUNCH_PROCESSING_H
#define BUGPACK_BASE_PUNCH_PROCESSING_H

#include <stdint.h>
#include <stdbool.h>

#define PUNCH_BUFFER_SIZE 12

void get_punch_values_from_data(uint8_t *punch_data);
void process_punch(uint8_t *punch_data);
void initialize_punch_processing();

typedef struct punch_data_t {
    uint32_t si_number;
    char iso8601_time[21];
} punch_data_t;

#endif //BUGPACK_BASE_PUNCH_PROCESSING_H
