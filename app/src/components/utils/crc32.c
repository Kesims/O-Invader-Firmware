#include <stddef.h>
#include "crc32.h"


uint32_t calculate_crc32(const uint8_t *data, uint32_t length) {
    uint32_t crc = 0xFFFFFFFF;
    if(data == NULL || length == 0){
        return 0;
    }

    for (uint32_t i = 0; i < length; i++) {
        crc ^= data[i];

        for (uint32_t j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ CRC32_POLYNOMIAL;
            } else {
                crc = (crc >> 1);
            }
        }
    }

    return crc;
}