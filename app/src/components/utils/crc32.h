#ifndef BUGPACK_BASE_CRC32_H
#define BUGPACK_BASE_CRC32_H

#include <stdint.h>

#define CRC32_POLYNOMIAL 0xEDB88320
uint32_t calculate_crc32(const uint8_t *data, uint32_t length);

#endif //BUGPACK_BASE_CRC32_H
