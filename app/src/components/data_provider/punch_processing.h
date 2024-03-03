#ifndef BUGPACK_BASE_PUNCH_PROCESSING_H
#define BUGPACK_BASE_PUNCH_PROCESSING_H

#include <stdint.h>

#define PUNCH_BUFFER_SIZE 12

void get_punch_values_from_data(uint8_t *punch_data);
void process_punch(uint8_t *punch_data);
void initialize_punch_processing();

#endif //BUGPACK_BASE_PUNCH_PROCESSING_H
