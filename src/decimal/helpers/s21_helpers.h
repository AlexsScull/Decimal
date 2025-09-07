/**
 * @file s21_helpers.h
 * @brief Вспомогательные функции для работы с decimal
 * @details Содержит функции для работы с битами, масштабирования и другие
 * утилиты
 */

#ifndef S21_HELPERS_H
#define S21_HELPERS_H

#include "../s21_decimal.h"

/* ОПЕРАЦИИ С БИТАМИ */

/**
 * @brief Получает значение бита в указанной позиции decimal числа
 * @param value - структура decimal, из которой читаем бит
 * @param position - позиция бита (0-127), где:
 *                  0 - младший бит мантиссы (bits[0])
 *                  127 - старший бит (знак в bits[3])
 * @return значение бита (0 или 1), или 0 при неверной позиции
 */
int get_bit(s21_decimal value, int position);

/**
 * @brief Устанавливает значение бита в указанной позиции decimal числа
 * @param value - указатель на структуру decimal для изменения
 * @param position - позиция бита (0-127), где:
 *                  0 - младший бит мантиссы (bits[0])
 *                  127 - старший бит (знак в bits[3])
 * @param bit - значение для установки (0 или 1)
 */
void set_bit(s21_decimal *value, int position, int bit);

/**
 * @brief Получает знак decimal числа
 * @param value Decimal число
 * @return Знак числа (0 - положительный, 1 - отрицательный)
 *
 */
int get_sign(s21_decimal value);

/**
 * @brief Устанавливает знак decimal числа
 * @param value Указатель на decimal число
 * @param sign Знак числа (0 - положительный, 1 - отрицательный)
 *
 */
void set_sign(s21_decimal *value, int sign);

/**
 * @brief Получает масштаб decimal числа
 * @param value Decimal число
 * @return Масштаб (степень 10, от 0 до 28)
 */
int get_scale(s21_decimal value);

/**
 * @brief Устанавливает масштаб decimal числа
 * @param value Указатель на decimal число
 * @param scale Масштаб (степень 10, от 0 до 28)
 */
void set_scale(s21_decimal *value, int scale);

/* СЛУЖЕБНЫЕ ФУНКЦИИ */

/**
 * @brief Проверяет, является ли decimal число нулем
 * @param value Decimal число
 * @return 1 если число равно нулю, иначе 0
 */
int s21_is_zero(s21_decimal value);

/**
 * @brief Обнуляет decimal число
 * @param value Указатель на decimal число
 */
void s21_zero_decimal(s21_decimal *value);

/**
 * @brief Инициализирует decimal нулевым значением
 * @return Возвращает decimal, равный нулю
 */
s21_decimal s21_decimal_init_zero(void);

/**
 * @brief Копирует decimal число
 * @param src Исходное decimal число
 * @param dest Указатель на целевое decimal число
 */
void s21_copy_decimal(s21_decimal src, s21_decimal *dest);

/* ОПЕРАЦИИ С МАСШТАБОМ */

/**
 * @brief Выравнивает масштабы двух decimal чисел
 * @param a Указатель на первое decimal число
 * @param b Указатель на второе decimal число
 * @details Умножает число с меньшим масштабом на соответствующую степень 10
 */
void s21_align_scales(s21_decimal *a, s21_decimal *b);

/**
 * @brief Умножает decimal число на степень 10
 * @param value Указатель на decimal число
 * @param power Степень 10
 * @return Код ошибки (s21_error_code)
 */
int s21_multiply_by_power10(s21_decimal *value, int power);

/**
 * @brief Делит decimal число на степень 10
 * @param value Указатель на decimal число
 * @param power Степень 10
 * @return Код ошибки (s21_error_code)
 */
int s21_divide_by_power10(s21_decimal *value, int power);

/**
 * @brief Нормализует decimal число (удаляет конечные нули)
 * @param value Указатель на decimal число
 * @return Код ошибки (s21_error_code)
 */
int s21_normalize(s21_decimal *value);

/* ОПЕРАЦИИ ОКРУГЛЕНИЯ */

/**
 * @brief Выполняет банковское округление decimal числа
 * @param value Указатель на decimal число
 * @param precision Точность округления
 * @details Округление до ближайшего четного числа при равенстве расстояний
 */
void s21_bank_round(s21_decimal *value, int precision);

#endif