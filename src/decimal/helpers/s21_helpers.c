#include "s21_helpers.h"

// bits[3]:
//  [31]      [30 - 24]         [23 - 16]             [15 - 0]
// ┌────┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐
// │знак│  не используется  │     МАСШТАБ   │      не используется      │
// └────┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┘

int get_bit(s21_decimal value, int position) {
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

void set_bit(s21_decimal *value, int position, int bit) {
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

int get_sign(s21_decimal value) {
  // Знак находится в 127-м бите (31-й бит в bits[3])
  return get_bit(value, 127);
}
void set_sign(s21_decimal *value, int sign) {
  // Знак находится в 127-м бите (31-й бит в bits[3])
  // sign & 1 - гарантируем что sign будет только 0 или 1
  set_bit(value, 127, sign & 1);
}

int get_scale(s21_decimal value) {
  //  Масштаб находится в битах 112-119 (16-23 биты в bits[3])
  //  1. value.bits[3] >> 16 - сдвигаем чтобы биты 16-23 стали младшими
  //  2. & 0xFF - маскируем только младшие 8 битов (0-255)
  return (value.bits[3] >> 16) & 0xFF;
}
void set_scale(s21_decimal *value, int scale) {
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

void s21_align_scales(s21_decimal *a, s21_decimal *b) {}
int s21_multiply_by_power10(s21_decimal *value, int power) {}
int s21_divide_by_power10(s21_decimal *value, int power) {}
int s21_normalize(s21_decimal *value) {}

void s21_bank_round(s21_decimal *value, int precision) {}