#ifndef S21_HELPERS_H
#define S21_HELPERS_H

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


static inline int s21_get_sign(const s21_decimal *d) {
  return (d->bits[3] >> 31) & 1U;
}

static inline int s21_get_scale(const s21_decimal *d) {
  return (d->bits[3] >> 16) & 0xFFU;
}

static inline void s21_set_sign(s21_decimal *d, int sign) {
  d->bits[3] = (d->bits[3] & ~(1U << 31)) | ((!!sign) << 31);
}

static inline void s21_set_scale(s21_decimal *d, int scale) {
  d->bits[3] = (d->bits[3] & ~(0xFFU << 16)) | ((scale & 0xFFU) << 16);
}

static inline int s21_validate_unused_bits(const s21_decimal *d) {
  return ((d->bits[3] & 0x7FFF0000) == (s21_get_scale(d) << 16)) &&
         ((d->bits[3] & 0x0000FFFF) == 0);
}

int s21_get_bit(s21_decimal value, int position);
void s21_set_bit(s21_decimal *value, int position, int bit);
void s21_zero_decimal(s21_decimal *value);
void s21_copy(s21_decimal value, s21_decimal *result);
s21_decimal s21_decimal_init_zero(void);
s21_decimal decimal_from_parts(int low, int mid, int high, int scale, int sign);
// int s21_normalize(s21_decimal *value_1, s21_decimal *value_2);
int s21_normalize(s21_decimal *value);
int s21_align_scales(s21_decimal *a, s21_decimal *b);
int s21_divide_by_power10(s21_decimal *value, int power);
uint32_t s21_divide_by_10(s21_decimal *value);
int s21_multiply_by_power10(s21_decimal *value, int power);
int s21_multiply_by_10(s21_decimal *value);
int s21_add_one(s21_decimal *value);

#endif