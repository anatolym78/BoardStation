/**
 * @file crc8.h
 * @brief Объявление функций для расчета контрольной суммы CRC8
 */
#pragma once

#include <cstddef>
#include <cstdint>

/**
 * @brief Рассчитать контрольную сумму CRC-8/ROHC
 * @param data Указатель на массив данных
 * @param len Длинна массива @p data
 * @return Рассчитанная контрольная сумма CRC-8/ROHC
 */
uint8_t crc8(const uint8_t *data, std::size_t len);
