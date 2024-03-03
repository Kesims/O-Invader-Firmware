#ifndef BUGPACK_BASE_PUNCH_PROVIDER_H
#define BUGPACK_BASE_PUNCH_PROVIDER_H

#include <stdint.h>
#include "punch_processing.h"

#define PUNCH_STORAGE_SIZE 128
void store_punch(punch_data_t new_punch);
int retrieve_next_punch();

#endif //BUGPACK_BASE_PUNCH_PROVIDER_H
