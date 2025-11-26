/**
 * @file crc8.cpp
 * @brief Определение функций для расчета контрольной суммы CRC8
 */
#include "crc8.h"

uint8_t crc8(const uint8_t *data, std::size_t len) {
    static constexpr uint8_t k_Init = 0xff;
    static constexpr uint8_t k_Poly = 0x07;
    static constexpr uint8_t k_Xor = 0x00;

    uint8_t checksum = k_Init;

    for (std::size_t i = 0; i < len; ++i) {
        checksum ^= data[i];

        for (uint8_t bit = 0; bit < 8; ++bit) {
            if (checksum & 0x80) {
                checksum = (checksum << 1) ^ k_Poly;
            }
            else {
                checksum <<= 1;
            }
        }
    }

    return checksum ^ k_Xor;
}
