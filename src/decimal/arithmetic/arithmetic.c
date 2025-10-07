#include "../s21_decimal.h"

// res.bits[0] = value_1.bits[0] + value_2.bits[0] 100 + 110 = 1010
// обработка переполнения                                       010
// res.bits[1] = value_1.bits[1] + value_2.bits[1]
// если было переполнения res.bits[1] += 1;                  + 1
// обработка переполнения
// res.bits[2] = value_1.bits[2] + value_2.bits[2]
// если было переполнения res.bits[1] += 1;
// обработка переполнения                              return S21_INF

// s21_add проверить знаки, если 1 (-) то перебросить в s21_sub если оба (-) то
// ничего s21_sub проверить знаки, если 1 (-) то перебросить в s21_add

// + (+) + сложение
// + (+) - отрицание
// - (+) + отрицание
// - (+) - сложение

// + (-) + отрицание
// + (-) - сложение
// - (-) + сложение
// - (-) - отрицание

// знак может измениться при отрицание

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  // //  здесь должна быть проверка на NULL и корректность s21_decimal
  // if (result == NULL || s21_validate_unused_bits(&value_1) ||
  //     s21_validate_unused_bits(&value_2))
  //   return S21_CONV_ERR;

  // // проверка знаков, при необходимости перебросить в s21_sub
  // if (s21_get_sign(&value_1) ^ s21_get_sign(&value_2)) return s21_sub(value_1, value_2, result);

  // // s21_normalize(&v_1, &v_2)

  // // res.bits[0] = value_1.bits[0] + value_2.bits[0] 100 + 110 = 1010
  // // обработка переполнения                                       010
  // // res.bits[1] = value_1.bits[1] + value_2.bits[1]
  // // если было переполнения res.bits[1] += 1;                  + 1
  // // обработка переполнения
  // // res.bits[2] = value_1.bits[2] + value_2.bits[2]
  // // если было переполнения res.bits[1] += 1;
  // // обработка переполнения                              return S21_INF
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {}