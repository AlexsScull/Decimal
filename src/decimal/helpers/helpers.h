/**
 * @file s21_helpers.h
 * @brief Вспомогательные функции для работы с decimal
 * @details Содержит функции для работы с битами, масштабирования и другие
 * утилиты
 */

#ifndef S21_HELPERS_H
#define S21_HELPERS_H

#include <stdint.h>
#include "../s21_decimal.h"

// Определения для разных размеров int
#if __SIZEOF_INT__ == 2
#define S21_INT_MAX 32767
#define S21_INT_MIN (-32768)
#elif __SIZEOF_INT__ == 4
#define S21_INT_MAX 2147483647
#define S21_INT_MIN (-2147483648)
#elif __SIZEOF_INT__ == 8
#define S21_INT_MAX 9223372036854775807LL
#define S21_INT_MIN (-9223372036854775808LL)
#else
// Стандартные значения для 32-битных систем
#define S21_INT_MAX 2147483647
#define S21_INT_MIN (-2147483648)
#endif

#define S21_UINT32_MAX 4294967295U
#define S21_DECIMAL_MAX 79228162514264337593543950335ULL

/**
 * @brief Получает знак decimal числа
 * @param value Decimal число
 * @return Знак числа (0 - положительный, 1 - отрицательный)
 */
static inline int s21_get_sign(const s21_decimal *d) {
  return (d->bits[3] >> 31) & 1U;
}

/**
 * @brief Получает масштаб decimal числа
 * @param value Decimal число
 * @return Масштаб (степень 10, от 0 до 28)
 */
static inline int s21_get_scale(const s21_decimal *d) {
  return (d->bits[3] >> 16) & 0xFFU;
}

/**
 * @brief Устанавливает знак decimal числа
 * @param value Указатель на decimal число
 * @param sign Знак числа (0 - положительный, 1 - отрицательный)
 */
static inline void s21_set_sign(s21_decimal *d, int sign) {
  d->bits[3] = (d->bits[3] & ~(1U << 31)) | ((!!sign) << 31);
}

/**
 * @brief Устанавливает масштаб decimal числа
 * @param value Указатель на decimal число
 * @param scale Масштаб (степень 10, от 0 до 28)
 */
static inline void s21_set_scale(s21_decimal *d, int scale) {
  d->bits[3] = (d->bits[3] & ~(0xFFU << 16)) | ((scale & 0xFFU) << 16);
}

static inline int s21_validate_unused_bits(const s21_decimal *d) {
  return ((d->bits[3] & 0x7FFF0000) == (s21_get_scale(d) << 16)) &&
         ((d->bits[3] & 0x0000FFFF) == 0);
}

/**
 * @brief Создает и возвращает decimal с нулевым значением
 * @details Функция инициализирует все биты структуры нулями.
 * @return Инициализированная нулевая структура decimal
 */
s21_decimal s21_decimal_zero(void);

// ==================== ARITHMETIC OPERATIONS ====================

int s21_align_scales(s21_decimal *a, s21_decimal *b);
int s21_multiply_by_power10(s21_decimal *value, int power);
int s21_divide_by_power10(s21_decimal *value, int power);

// ==================== ROUNDING OPERATIONS ====================

int s21_add_one(s21_decimal *value);
int s21_normalize(s21_decimal *value);
void s21_bank_round(s21_decimal *value, int target_precision);
int s21_bank_round_remainder(s21_decimal *value, uint32_t remainder);

#endif