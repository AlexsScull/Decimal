#include "helpers.h"
#include <stdlib.h>
#include <limits.h>

// ==================== BIT OPERATIONS ====================

int s21_get_bit(s21_decimal value, int position) {
  if (position < 0 || position > 127) return 0;

  int word_index = position / 32;
  int bit_offset = position % 32;

  return (value.bits[word_index] >> bit_offset) & 1;
}

void s21_set_bit(s21_decimal *value, int position, int bit) {
  if (!value || position < 0 || position > 127) return;

  int word_index = position / 32;
  int bit_offset = position % 32;

  if (bit)
    value->bits[word_index] |= (1u << bit_offset);
  else
    value->bits[word_index] &= ~(1u << bit_offset);
}

void s21_copy(s21_decimal value, s21_decimal *result) {*result = value;}

void s21_zero_decimal(s21_decimal *value) {
  if (!value) return;

  for (int i = 0; i < 4; i++) {
    value->bits[i] = 0;
  }
}

s21_decimal s21_decimal_init_zero(void) { 
    return (s21_decimal){{0, 0, 0, 0}}; 
}

s21_decimal decimal_from_parts(int low, int mid, int high, int scale, int sign) {
    s21_decimal result = {{low, mid, high, 0}};
    s21_set_sign(&result, sign);
    s21_set_scale(&result, scale);
    return result;
}

int s21_normalize(s21_decimal *value) {
  if (!value) return S21_COMPARISON_FALSE;
  
  int is_zero = (value->bits[0] == 0 && value->bits[1] == 0 && value->bits[2] == 0);
  if (is_zero) {
    s21_set_scale(value, 0);
    s21_set_sign(value, 0);
    return S21_COMPARISON_TRUE;
  }

  int original_sign = s21_get_sign(value);
  int current_scale = s21_get_scale(value);
  s21_decimal temp = *value;

  s21_set_sign(&temp, 0);

  while (current_scale > 0) {
    s21_decimal before_division = temp;
    uint32_t remainder = s21_divide_by_10(&temp);

    if (remainder != 0) {
      temp = before_division;
      break;
    }

    current_scale--;
  }

  *value = temp;
  s21_set_scale(value, current_scale);
  s21_set_sign(value, original_sign);

  return S21_COMPARISON_TRUE;
}

int s21_align_scales(s21_decimal *a, s21_decimal *b) {
  if (!a || !b) return S21_COMPARISON_FALSE;

  int scale_a = s21_get_scale(a);
  int scale_b = s21_get_scale(b);

  if (scale_a == scale_b) return S21_COMPARISON_TRUE;

  s21_decimal *smaller_scale = (scale_a < scale_b) ? a : b;
  int scale_diff = abs(scale_a - scale_b);

  int error = s21_multiply_by_power10(smaller_scale, scale_diff);

  if (!error) {
    int new_scale = (scale_a < scale_b) ? scale_a + scale_diff : scale_b + scale_diff;
    s21_set_scale(smaller_scale, new_scale);
  }

  return error;
}

int s21_divide_by_power10(s21_decimal *value, int power) {
  if (!value || power < 0) return S21_COMPARISON_FALSE;
  if (power == 0) return S21_COMPARISON_TRUE;
  if (power > 28) return s21_get_sign(value) ? S21_ARITHMETIC_NEG_INF  : S21_ARITHMETIC_INF;

  for (int i = 0; i < power; i++) {
    s21_divide_by_10(value);
  }

  return S21_COMPARISON_TRUE;
}

uint32_t s21_divide_by_10(s21_decimal *value) {
  uint64_t remainder = 0;

  for (int i = 2; i >= 0; i--) {
    uint64_t current_value = ((uint64_t)remainder << 32) | value->bits[i];
    value->bits[i] = (uint32_t)(current_value / 10);
    remainder = current_value % 10;
  }

  return (uint32_t)remainder;
}

int s21_multiply_by_power10(s21_decimal *value, int power) {
  if (!value || power < 0) return S21_COMPARISON_FALSE;
  if (power == 0) return S21_COMPARISON_TRUE;
  if (power > 28) return s21_get_sign(value) ? S21_ARITHMETIC_NEG_INF  : S21_ARITHMETIC_INF;

  for (int i = 0; i < power; i++) {
    if (s21_multiply_by_10(value) != S21_COMPARISON_TRUE) {
      return S21_ARITHMETIC_INF;
    }
  }
  return S21_COMPARISON_TRUE;
}

int s21_multiply_by_10(s21_decimal *value) {
  uint64_t carry = 0;

  for (int i = 0; i < 3; i++) {
    uint64_t temp = (uint64_t)value->bits[i] * 10 + carry;
    value->bits[i] = (uint32_t)(temp & 0xFFFFFFFF);
    carry = temp >> 32;
  }

  return (carry == 0) ? S21_COMPARISON_TRUE : S21_ARITHMETIC_INF;
}

int s21_add_one(s21_decimal *value) {
  if (!value) return S21_COMPARISON_FALSE;

  for (int i = 0; i < 3; i++) {
    if (value->bits[i] < S21_UINT32_MAX) {
      value->bits[i]++;
      return S21_COMPARISON_TRUE;
    } else {
      value->bits[i] = 0;
    }
  }
  return S21_ARITHMETIC_INF;
}
