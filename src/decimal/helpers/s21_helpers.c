#include "s21_helpers.h"

int get_bit(s21_decimal value, int position) {}
void set_bit(s21_decimal *value, int position, int bit) {}
int get_sign(s21_decimal value) {}
void set_sign(s21_decimal *value, int sign) {}
int get_scale(s21_decimal value) {}
void set_scale(s21_decimal *value, int scale) {}

int s21_is_zero(s21_decimal value) {}
void s21_zero_decimal(s21_decimal *value) {}
void s21_copy_decimal(s21_decimal src, s21_decimal *dest) {}

void s21_align_scales(s21_decimal *a, s21_decimal *b) {}
int s21_multiply_by_power10(s21_decimal *value, int power) {}
int s21_divide_by_power10(s21_decimal *value, int power) {}
int s21_normalize(s21_decimal *value) {}

void s21_bank_round(s21_decimal *value, int precision) {}