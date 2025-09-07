/**
 * @file s21_decimal.h
 * @brief Заголовочный файл для реализации типа decimal
 * @details Реализация собственного типа данных decimal для точных десятичных
 * вычислений
 */

#ifndef S21_DECIMAL_H
#define S21_DECIMAL_H

#include <stdint.h>
#include <stdio.h>

/**
 * @brief Коды ошибок для операций с decimal
 */
typedef enum {
  S21_OK = 0,  /**< Операция выполнена успешно */
  S21_INF = 1, /**< Результат слишком велик или равен бесконечности */
  S21_NEG_INF =
      2, /**< Результат слишком мал или равен отрицательной бесконечности */
  S21_ZERO_DIV = 3, /**< Деление на ноль */
  S21_CONV_ERR = 4  /**< Ошибка конвертации */
} s21_error_code;

/**
 * @brief Коды возврата для операторов сравнения
 */
typedef enum {
  S21_FALSE = 0, /**< Ложь */
  S21_TRUE = 1   /**< Истина */
} s21_bool;

/**
 * @brief Структура decimal для представления десятичных чисел с фиксированной
 * точкой
 * @details Число представляется как целое значение (мантисса) и масштаб
 * (степень 10)
 */
typedef struct {
  int bits[4]; /**< Массив из четырех 32-битных целых чисел для хранения числа
                */
} s21_decimal;

/* АРИФМЕТИЧЕСКИЕ ОПЕРАТОРЫ */

/**
 * @brief Выполняет сложение двух decimal чисел
 * @param value_1 Первое слагаемое
 * @param value_2 Второе слагаемое
 * @param result Указатель на результат сложения
 * @return Код ошибки (s21_error_code)
 */
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

/**
 * @brief Выполняет вычитание двух decimal чисел
 * @param value_1 Уменьшаемое
 * @param value_2 Вычитаемое
 * @param result Указатель на результат вычитания
 * @return Код ошибки (s21_error_code)
 */
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

/**
 * @brief Выполняет умножение двух decimal чисел
 * @param value_1 Первый множитель
 * @param value_2 Второй множитель
 * @param result Указатель на результат умножения
 * @return Код ошибки (s21_error_code)
 */
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

/**
 * @brief Выполняет деление двух decimal чисел
 * @param value_1 Делимое
 * @param value_2 Делитель
 * @param result Указатель на результат деления
 * @return Код ошибки (s21_error_code)
 */
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

/* ОПЕРАТОРЫ СРАВНЕНИЯ */

/**
 * @brief Проверяет, меньше ли первое число второго
 * @param a Первое decimal число
 * @param b Второе decimal число
 * @return S21_TRUE если a < b, иначе S21_FALSE
 */
int s21_is_less(s21_decimal a, s21_decimal b);

/**
 * @brief Проверяет, меньше или равно первое число второго
 * @param a Первое decimal число
 * @param b Второе decimal число
 * @return S21_TRUE если a <= b, иначе S21_FALSE
 */
int s21_is_less_or_equal(s21_decimal a, s21_decimal b);

/**
 * @brief Проверяет, больше ли первое число второго
 * @param a Первое decimal число
 * @param b Второе decimal число
 * @return S21_TRUE если a > b, иначе S21_FALSE
 */
int s21_is_greater(s21_decimal a, s21_decimal b);

/**
 * @brief Проверяет, больше или равно первое число второго
 * @param a Первое decimal число
 * @param b Второе decimal число
 * @return S21_TRUE если a >= b, иначе S21_FALSE
 */
int s21_is_greater_or_equal(s21_decimal a, s21_decimal b);

/**
 * @brief Проверяет, равны ли два числа
 * @param a Первое decimal число
 * @param b Второе decimal число
 * @return S21_TRUE если a == b, иначе S21_FALSE
 */
int s21_is_equal(s21_decimal a, s21_decimal b);

/**
 * @brief Проверяет, не равны ли два числа
 * @param a Первое decimal число
 * @param b Второе decimal число
 * @return S21_TRUE если a != b, иначе S21_FALSE
 */
int s21_is_not_equal(s21_decimal a, s21_decimal b);

/* ПРЕОБРАЗОВАТЕЛИ */

/**
 * @brief Преобразует целое число int в decimal
 * @param src Исходное целое число
 * @param dst Указатель на decimal для сохранения результата
 * @return Код ошибки (s21_error_code)
 */
int s21_from_int_to_decimal(int src, s21_decimal *dst);

/**
 * @brief Преобразует число float в decimal
 * @param src Исходное число float
 * @param dst Указатель на decimal для сохранения результата
 * @return Код ошибки (s21_error_code)
 */
int s21_from_float_to_decimal(float src, s21_decimal *dst);

/**
 * @brief Преобразует decimal в целое число int
 * @param src Исходное decimal число
 * @param dst Указатель на int для сохранения результата
 * @return Код ошибки (s21_error_code)
 */
int s21_from_decimal_to_int(s21_decimal src, int *dst);

/**
 * @brief Преобразует decimal в число float
 * @param src Исходное decimal число
 * @param dst Указатель на float для сохранения результата
 * @return Код ошибки (s21_error_code)
 */
int s21_from_decimal_to_float(s21_decimal src, float *dst);

/* ДРУГИЕ ФУНКЦИИ */

/**
 * @brief Округляет decimal до ближайшего целого в сторону отрицательной
 * бесконечности
 * @param value Исходное decimal число
 * @param result Указатель на decimal для сохранения результата
 * @return Код ошибки (s21_error_code)
 */
int s21_floor(s21_decimal value, s21_decimal *result);

/**
 * @brief Округляет decimal до ближайшего целого числа
 * @param value Исходное decimal число
 * @param result Указатель на decimal для сохранения результата
 * @return Код ошибки (s21_error_code)
 */
int s21_round(s21_decimal value, s21_decimal *result);

/**
 * @brief Отбрасывает дробную часть decimal числа
 * @param value Исходное decimal число
 * @param result Указатель на decimal для сохранения результата
 * @return Код ошибки (s21_error_code)
 */
int s21_truncate(s21_decimal value, s21_decimal *result);

/**
 * @brief Умножает decimal число на -1 (меняет знак)
 * @param value Исходное decimal число
 * @param result Указатель на decimal для сохранения результата
 * @return Код ошибки (s21_error_code)
 */
int s21_negate(s21_decimal value, s21_decimal *result);

#endif