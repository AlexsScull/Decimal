#include "helpers.h"
#include <stdlib.h>
#include <limits.h>

// ==================== FORWARD DECLARATIONS ====================

static int s21_multiply_by_10(s21_decimal *value);
static uint32_t s21_divide_by_10(s21_decimal *value);
static uint32_t s21_get_last_digit(s21_decimal value);

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

// ==================== DECIMAL MANIPULATION ====================

void s21_zero_decimal(s21_decimal *value) {
  if (!value) return;

  for (int i = 0; i < 4; i++) {
    value->bits[i] = 0;
  }
}

s21_decimal s21_decimal_init_zero(void) { 
    return (s21_decimal){{0, 0, 0, 0}}; 
}

// ==================== ARITHMETIC OPERATIONS ====================

static int s21_multiply_by_10(s21_decimal *value) {
  uint64_t carry = 0;

  for (int i = 0; i < 3; i++) {
    uint64_t temp = (uint64_t)value->bits[i] * 10 + carry;
    value->bits[i] = (uint32_t)(temp & 0xFFFFFFFF);
    carry = temp >> 32;
  }

  return (carry == 0) ? S21_OK : S21_INF;
}

int s21_multiply_by_power10(s21_decimal *value, int power) {
  if (!value || power < 0) return S21_ERR;
  if (power == 0) return S21_OK;
  if (power > 28) return s21_get_sign(value) ? S21_NEG_INF : S21_INF;

  for (int i = 0; i < power; i++) {
    if (s21_multiply_by_10(value) != S21_OK) {
      return S21_INF;
    }
  }
  return S21_OK;
}

static uint32_t s21_divide_by_10(s21_decimal *value) {
  uint64_t remainder = 0;

  for (int i = 2; i >= 0; i--) {
    uint64_t current_value = ((uint64_t)remainder << 32) | value->bits[i];
    value->bits[i] = (uint32_t)(current_value / 10);
    remainder = current_value % 10;
  }

  return (uint32_t)remainder;
}

int s21_divide_by_power10(s21_decimal *value, int power) {
  if (!value || power < 0) return S21_ERR;
  if (power == 0) return S21_OK;
  if (power > 28) return s21_get_sign(value) ? S21_NEG_INF : S21_INF;

  for (int i = 0; i < power; i++) {
    s21_divide_by_10(value);
  }

  return S21_OK;
}

int s21_align_scales(s21_decimal *a, s21_decimal *b) {
  if (!a || !b) return S21_ERR;

  int scale_a = s21_get_scale(a);
  int scale_b = s21_get_scale(b);

  if (scale_a == scale_b) return S21_OK;

  s21_decimal *smaller_scale = (scale_a < scale_b) ? a : b;
  int scale_diff = abs(scale_a - scale_b);

  int error = s21_multiply_by_power10(smaller_scale, scale_diff);

  if (!error) {
    int new_scale = (scale_a < scale_b) ? scale_a + scale_diff : scale_b + scale_diff;
    s21_set_scale(smaller_scale, new_scale);
  }

  return error;
}

// ==================== ROUNDING OPERATIONS ====================

static uint32_t s21_get_last_digit(s21_decimal value) {
  s21_decimal copy = value;
  return s21_divide_by_10(&copy);
}

int s21_add_one(s21_decimal *value) {
  if (!value) return S21_ERR;

  for (int i = 0; i < 3; i++) {
    if (value->bits[i] < S21_UINT32_MAX) {
      value->bits[i]++;
      return S21_OK;
    } else {
      value->bits[i] = 0;
    }
  }
  return S21_INF;
}

int s21_normalize(s21_decimal *value) {
  if (!value) return S21_ERR;
  
  // Проверка на ноль через прямое сравнение битов
  int is_zero = (value->bits[0] == 0 && value->bits[1] == 0 && value->bits[2] == 0);
  if (is_zero) {
    s21_set_scale(value, 0);
    s21_set_sign(value, 0);
    return S21_OK;
  }

  int original_sign = s21_get_sign(value);
  int current_scale = s21_get_scale(value);
  s21_decimal temp = *value;

  s21_set_sign(&temp, 0);  // Work with positive number

  // Remove trailing zeros by dividing by 10
  while (current_scale > 0) {
    s21_decimal before_division = temp;
    uint32_t remainder = s21_divide_by_10(&temp);

    if (remainder != 0) {
      temp = before_division;  // Restore previous value
      break;
    }

    current_scale--;
  }

  *value = temp;
  s21_set_scale(value, current_scale);
  s21_set_sign(value, original_sign);

  return S21_OK;
}

void s21_bank_round(s21_decimal *value, int target_precision) {
  if (!value) return;

  int current_scale = s21_get_scale(value);
  if (current_scale <= target_precision) return;

  int divisions_needed = current_scale - target_precision;
  int original_sign = s21_get_sign(value);
  uint32_t last_remainder = 0;
  int has_non_zero_remainder = 0;

  s21_set_sign(value, 0);  // Work with positive number

  // Divide to remove extra decimal places
  for (int i = 0; i < divisions_needed; i++) {
    uint32_t remainder = s21_divide_by_10(value);

    if (i == divisions_needed - 1) {
      last_remainder = remainder;
    } else if (remainder != 0) {
      has_non_zero_remainder = 1;
    }
  }

  // Apply banking rounding rules
  if (last_remainder > 5 ||
      (last_remainder == 5 &&
       (has_non_zero_remainder || s21_get_last_digit(*value) % 2 == 1))) {
    s21_add_one(value);
  }

  s21_set_scale(value, target_precision);
  s21_set_sign(value, original_sign);
}

int s21_bank_round_remainder(s21_decimal *value, uint32_t remainder) {
  if (!value) return S21_ERR;

  if (remainder > 5 ||
      (remainder == 5 && s21_get_last_digit(*value) % 2 == 1)) {
    return s21_add_one(value);
  }

  return S21_OK;
}