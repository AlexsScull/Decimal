#include "s21_helpers.h"

// bits[3]:
//  [31]      [30 - 24]         [23 - 16]             [15 - 0]
// ┌────┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐
// │знак│  не используется  │     МАСШТАБ   │      не используется      │
// └────┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┘

int s21_get_bit(s21_decimal value, int position) {
  if (position < 0 || position > 127) return 0;

  // Определяем в каком слове массива bits находится нужный бит
  // Деление на 32: position = 45 → 45/32 = 1 (bits[1])
  int word = position / 32;  // 0, 1, 2 или 3

  // Определяем позицию бита внутри слова (остаток от деления на 32)
  // position = 45 → 45 % 32 = 13 (13-й бит в bits[1])
  int bit_in_word = position % 32;  // 0-31

  // Получаем нужный бит:
  // 1. value.bits[word] - берем нужное 32-битное слово
  // 2. >> bit_in_word - сдвигаем вправо, чтобы нужный бит стал младшим
  // 3. & 1 - маскируем все биты кроме младшего
  return (value.bits[word] >> bit_in_word) & 1;
}

void s21_set_bit(s21_decimal *value, int position, int bit) {
  if (position < 0 || position > 127) return;

  // Определяем слово и позицию бита (аналогично get_bit)
  int word = position / 32;
  int bit_in_word = position % 32;

  if (bit) {
    // Установка бита в 1:
    // 1u << bit_in_word - создаем маску с единицей в нужной позиции
    // |= - побитовое ИЛИ устанавливает бит в 1
    value->bits[word] |= (1u << bit_in_word);
  } else {
    // Установка бита в 0:
    // ~(1u << bit_in_word) - создаем маску с нулем в нужной позиции
    // &= - побитовое И сбрасывает бит в 0
    value->bits[word] &= ~(1u << bit_in_word);
  }
}

int s21_get_sign(s21_decimal value) {
  // Знак находится в 127-м бите (31-й бит в bits[3])
  return s21_get_bit(value, 127);
}
void s21_set_sign(s21_decimal *value, int sign) {
  // Знак находится в 127-м бите (31-й бит в bits[3])
  // sign & 1 - гарантируем что sign будет только 0 или 1
  s21_set_bit(value, 127, sign & 1);
}

int s21_get_scale(s21_decimal value) {
  //  Масштаб находится в битах 112-119 (16-23 биты в bits[3])
  //  1. value.bits[3] >> 16 - сдвигаем чтобы биты 16-23 стали младшими
  //  2. & 0xFF - маскируем только младшие 8 битов (0-255)
  return (value.bits[3] >> 16) & 0xFF;
}
void s21_set_scale(s21_decimal *value, int scale) {
  // Очищаем биты масштаба (16-23):
  // 0xFF << 16 = 00000000 11111111 00000000 00000000
  // ~(0xFF << 16) = 11111111 00000000 11111111 11111111
  // &= применяет эту маску, обнуляя биты 16-23
  // Этой операцией мы "зануляем" только ячейки 16-23, остальные не трогаем
  value->bits[3] &= ~(0xFF << 16);
  // Устанавливаем новые значения:
  // scale & 0xFF = гарантируем, что число влезет в 8 битов (0-255)
  // << 16 = сдвигаем число в ячейки 16-23
  // |= - устанавливаем эти биты
  // Пример: scale = 5 → 00000000 00000101 00000000 00000000
  value->bits[3] |= (scale & 0xFF) << 16;
}

int s21_is_zero(s21_decimal value) {
  return value.bits[0] == 0 && value.bits[1] == 0 && value.bits[2] == 0;
}

void s21_zero_decimal(s21_decimal *value) {
  if (value) {
    value->bits[0] = 0;
    value->bits[1] = 0;
    value->bits[2] = 0;
    value->bits[3] = 0;
  }
}

s21_decimal s21_decimal_init_zero(void) { return (s21_decimal){{0, 0, 0, 0}}; }

void s21_copy_decimal(s21_decimal src, s21_decimal *dest) {
  if (dest != NULL) {
    for (int i = 0; i < 4; i++) {
      dest->bits[i] = src.bits[i];
    }
  }
}

int s21_align_scales(s21_decimal *a, s21_decimal *b) {
  if (!a || !b) return S21_CONV_ERR;

  int error = S21_OK;

  int scale_a = s21_get_scale(*a);
  int scale_b = s21_get_scale(*b);

  if (scale_a < scale_b) {
    int diff = scale_b - scale_a;
    error = s21_multiply_by_power10(a, diff);
    if (!error) s21_set_scale(a, scale_a + diff);
  } else if (scale_b < scale_a) {
    int diff = scale_a - scale_b;
    error = s21_multiply_by_power10(b, diff);
    if (!error) s21_set_scale(b, scale_b + diff);
  }

  return error;
}

int s21_multiply_by_power10(s21_decimal *value, int power) {
  if (!value || power < 0) return S21_CONV_ERR;
  if (power == 0) return S21_OK;  // Умножение на 1
  if (power > 28)
    return s21_get_sign(*value) ? S21_NEG_INF
                                : S21_INF;  // Некорректная степень

  int error = S21_OK;
  for (int i = 0; i < power && !error; i++) {
    error = s21_multiply_by_10(value);
  }
  return error;
}

static int s21_multiply_by_10(s21_decimal *value) {
  // uint64_t для временных вычислений, чтобы обработать возможное переполнение
  uint64_t carry = 0;  // переменнная для переноса
  uint64_t temp;       // буфер

  // Умножаем младшую [0] среднюю [1] старшую [2] часть
  for (size_t i = 0; i < 3; i++) {
    temp = (uint64_t)value->bits[i] * 10 + carry;
    value->bits[i] = (uint32_t)(temp & 0xFFFFFFFF);
    carry = temp >> 32;
  }

  // Если есть перенос после старшей части - переполнение
  return carry == 0 ? S21_OK : S21_INF;
}

int s21_divide_by_power10(s21_decimal *value, int power) {
  if (!value || power < 0) return S21_CONV_ERR;
  if (power == 0) return S21_OK;  // Деление на 1
  if (power > 28)
    return s21_get_sign(*value) ? S21_NEG_INF
                                : S21_INF;  // Некорректная степень

  uint32_t remainder = 0;
  int error = S21_OK;

  for (int i = 0; i < power; i++) {
    remainder = s21_divide_by_10(value);
  }

  error = s21_bank_round_remainder(value, remainder);
  return error;
}

static uint32_t s21_divide_by_10(s21_decimal *value) {
  uint64_t remainder = 0;

  for (int i = 2; i >= 0; i--) {
    uint64_t current = ((uint64_t)remainder << 32) | (uint64_t)value->bits[i];
    value->bits[i] = (uint32_t)(current / 10);
    remainder = current % 10;
  }

  return (uint32_t)remainder;
}

static uint32_t s21_get_last_digit(s21_decimal value) {
  s21_decimal copy = value;
  return s21_divide_by_10(&copy);
}

int s21_add_one(s21_decimal *value) {
  for (int i = 0; i < 3; i++) {
    if (value->bits[i] < 0xFFFFFFFF) {
      value->bits[i]++;
      return S21_OK;
    } else {
      value->bits[i] = 0;
    }
  }
  return S21_INF;  // Переполнение
}

int s21_normalize(s21_decimal *value) {
  if (!value) return S21_CONV_ERR;
  if (s21_is_zero(*value)) {
    s21_set_scale(value, 0);
    s21_set_sign(value, 0);
    return S21_OK;
  }

  int sign = s21_get_sign(*value);
  int current_scale = s21_get_scale(*value);
  int new_scale = current_scale;
  s21_decimal temp = *value;
  s21_set_sign(&temp, 0);  // Работаем с положительным числом

  while (new_scale > 0) {
    uint32_t remainder = 0;
    s21_decimal copy = temp;
    remainder = s21_divide_by_10(&copy);

    if (remainder != 0) break;

    temp = copy;
    new_scale--;
  }

  *value = temp;
  s21_set_scale(value, new_scale);
  s21_set_sign(value, sign);
  return S21_OK;
}

void s21_bank_round(s21_decimal *value, int precision) {
  if (!value) return;

  int current_scale = s21_get_scale(*value);
  if (current_scale <= precision) return;

  int diff = current_scale - precision;
  int sign = s21_get_sign(*value);
  s21_set_sign(value, 0);  // Работаем с положительным числом

  uint32_t last_remainder = 0;
  int has_non_zero_remainder = 0;

  for (int i = 0; i < diff; i++) {
    uint32_t remainder = s21_divide_by_10(value);

    if (remainder != 0 && i < diff - 1) {
      has_non_zero_remainder = 1;
    }
    last_remainder = remainder;
  }

  // Банковское округление
  if (last_remainder > 5 ||
      (last_remainder == 5 &&
       (has_non_zero_remainder || (s21_get_last_digit(*value) % 2 == 1)))) {
    s21_add_one(value);
  }

  s21_set_scale(value, precision);
  s21_set_sign(value, sign);
}

int s21_bank_round_remainder(s21_decimal *value, uint32_t remainder) {
  if (!value) return S21_CONV_ERR;

  uint32_t last_digit = s21_get_last_digit(*value);
  int is_odd = last_digit % 2 == 1;

  if (remainder > 5 || (remainder == 5 && is_odd)) {
    return s21_add_one(value);
  }
  return S21_OK;
}