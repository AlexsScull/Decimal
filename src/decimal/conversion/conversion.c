#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "../s21_decimal.h"
#include "../helpers/helpers.h"

// Кэшированные значения для быстрого доступа
static const struct {
  uint64_t int_max;
  uint64_t int_min_abs;
} int_limits_cache = {.int_max = (uint64_t)S21_INT_MAX,
                      .int_min_abs = (uint64_t)(-((int64_t)S21_INT_MIN + 1)) + 1};

/* ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ */

/**
 * @brief Создает и возвращает decimal с нулевым значением (локальная версия)
 * @details Функция инициализирует все биты структуры нулями.
 * @return Инициализированная нулевая структура decimal
 */
static s21_decimal s21_decimal_zero_local(void) { 
    return (s21_decimal){{0, 0, 0, 0}}; 
}

/**
 * @brief Проверяет, является ли decimal нулевым значением
 * @param value Decimal число для проверки
 * @return S21_TRUE если значение равно нулю, иначе S21_FALSE
 */
static int s21_is_zero(s21_decimal value) {
    return value.bits[0] == 0 && value.bits[1] == 0 && value.bits[2] == 0;
}

/**
 * @brief Преобразует decimal в long double
 * @details Извлекает мантиссу из decimal и преобразует в long double
 * @param value Decimal число
 * @return Числовое значение в long double
 */
static long double s21_decimal_to_long_double(s21_decimal value) {
    long double result = 0.0L;
    result += (long double)value.bits[0];
    result += (long double)value.bits[1] * 4294967296.0L;
    result += (long double)value.bits[2] * 18446744073709551616.0L;
    return result;
}

/* ПРЕОБРАЗОВАНИЯ INT <-> DECIMAL */

/**
 * @brief Проверяет возможность преобразования decimal в int
 * @details Низкоуровневая функция. Вызывающая сторона должна обеспечить:
 *          - decimal должен быть усечен до целого числа (scale = 0)
 *          - Проверка указателей должна быть выполнена заранее
 *          Функция не проверяет указатели на NULL.
 * @param value Decimal число для проверки
 * @return S21_CONV_OK если преобразование возможно, иначе S21_CONV_ERR
 */
static int s21_can_convert_decimal_to_int(s21_decimal value) {
    int sign = s21_get_sign(&value);

    // Проверяем старшие биты на наличие переполнения
    if (value.bits[2] != 0 || value.bits[1] > S21_UINT32_MAX) {
        return S21_CONV_ERR;
    }

    // Извлекаем абсолютное значение
    uint64_t abs_value = (uint64_t)value.bits[0] | ((uint64_t)value.bits[1] << 32);

    // Проверяем диапазон в зависимости от знака
    if (sign == 0) {
        return (abs_value <= int_limits_cache.int_max) ? S21_CONV_OK : S21_CONV_ERR;
    } else {
        return (abs_value <= int_limits_cache.int_min_abs) ? S21_CONV_OK : S21_CONV_ERR;
    }
}

/**
 * @brief Преобразует decimal в int (низкоуровневая функция)
 * @details Вызывающая сторона должна обеспечить:
 *          - decimal должен быть усечен до целого числа (scale = 0)
 *          - decimal значение должно помещаться в диапазон int
 *          - Проверка переполнения должна быть выполнена заранее
 *          Функция не проверяет указатели на NULL и не проверяет переполнение.
 * @param value Decimal число для преобразования
 * @return Преобразованное целое число
 */
static int s21_convert_decimal_to_int(s21_decimal value) {
    int64_t result = (int64_t)value.bits[0] | ((int64_t)value.bits[1] << 32);
    if (s21_get_sign(&value) == 1) {
        result = -result;
    }
    return (int)result;
}

/**
 * @brief Преобразует int в decimal (низкоуровневая функция)
 * @details Вызывающая сторона должна обеспечить:
 *          - Указатель value должен быть валидным
 *          - Структура decimal должна быть инициализирована нулем
 *          Функция не проверяет указатели на NULL и не инициализирует структуру.
 * @param src Целое число для преобразования
 * @param value Указатель на decimal для сохранения результата
 */
static void s21_convert_int_to_decimal(int src, s21_decimal *value) {
    if (src == 0) return;

    int sign = (src < 0) ? 1 : 0;
    uint64_t abs_value;

    if (src == S21_INT_MIN) {
        abs_value = int_limits_cache.int_min_abs;
    } else {
        abs_value = (uint64_t)(sign ? -src : src);
    }

    value->bits[0] = abs_value & S21_UINT32_MAX;
    if (abs_value > S21_UINT32_MAX) {
        value->bits[1] = (abs_value >> 32) & S21_UINT32_MAX;
    }

    if (sign) {
        s21_set_sign(value, 1);
    }
}

/* ПРЕОБРАЗОВАНИЯ FLOAT <-> DECIMAL */

/**
 * @brief Проверяет возможность преобразования float в decimal
 * @details Низкоуровневая функция. Вызывающая сторона должна обеспечить:
 *          - Проверка указателей должна быть выполнена заранее
 *          Функция не проверяет указатели на NULL.
 * @param value Float число для проверки
 * @return S21_CONV_OK если преобразование возможно, иначе S21_CONV_ERR
 */
static int s21_can_convert_float_to_decimal(float value) {
    // Обработка специальных значений
    if (isnan(value) || isinf(value)) {
        return S21_CONV_ERR;
    }
    
    // Проверка граничных значений
    float abs_src = fabsf(value);
    if (abs_src < 1e-28f) {
        return S21_CONV_ERR;
    }
    if (abs_src > 7.922816e28f) {
        return S21_CONV_ERR;
    }
    
    return S21_CONV_OK;
}

/**
 * @brief Преобразует float в decimal с учетом точности (низкоуровневая функция)
 * @details Вызывающая сторона должна обеспечить:
 *          - Указатель value должен быть валидным
 *          - Структура decimal должна быть инициализирована нулем
 *          Функция не проверяет указатели на NULL и не инициализирует структуру.
 * @param src Исходное число float
 * @param value Указатель на decimal для сохранения результата
 * @return Код ошибки (S21_CONV_OK или S21_CONV_ERR)
 */
static int s21_convert_float_to_decimal_with_precision(float src, s21_decimal *value) {
    // Определяем знак
    int sign = (src < 0.0f) ? 1 : 0;
    double abs_value = fabs(src);
    
    // Обработка нуля
    if (abs_value == 0.0) {
        return S21_CONV_OK;
    }
    
    // Определяем порядок числа
    int exponent = (int)floor(log10(abs_value));
    
    // Масштабируем до 7 значащих цифр
    double factor = pow(10.0, 6 - exponent);
    double scaled = abs_value * factor;
    uint64_t scaled_int = (uint64_t)round(scaled);
    
    // Проверяем переполнение (максимальное значение decimal)
    if (scaled_int > S21_DECIMAL_MAX) {
        return S21_CONV_ERR;
    }
    
    // Устанавливаем значение в decimal
    value->bits[0] = scaled_int & 0xFFFFFFFF;
    value->bits[1] = (scaled_int >> 32) & 0xFFFFFFFF;
    value->bits[2] = 0;
    
    // Устанавливаем масштаб
    int scale = 6 - exponent;
    if (scale < 0) scale = 0;
    if (scale > 28) scale = 28;
    s21_set_scale(value, scale);
    
    // Устанавливаем знак
    if (sign) {
        s21_set_sign(value, 1);
    }
    
    return S21_CONV_OK;
}

/* ОСНОВНЫЕ ФУНКЦИИ ПРЕОБРАЗОВАНИЯ */

/**
 * @brief Преобразует целое число int в decimal
 * @details Вызывающая сторона должна обеспечить валидность указателя dst и decimal.
 *          Функция инициализирует dst нулевым значением перед преобразованием.
 * @param src Исходное целое число
 * @param dst Указатель на decimal для сохранения результата
 * @return Код ошибки (S21_CONV_OK или S21_CONV_ERR)
 */
int s21_from_int_to_decimal(int src, s21_decimal *dst) {
    if (dst == NULL) return S21_CONV_ERR;

    *dst = s21_decimal_zero_local();
    s21_convert_int_to_decimal(src, dst);

    return S21_CONV_OK;
}

/**
 * @brief Преобразует число float в decimal
 * @details Вызывающая сторона должна обеспечить валидность указателя dst.
 *          Функция обрабатывает специальные значения (NaN, бесконечности),
 *          проверяет границы значений и округляет до 7 значащих цифр.
 * @param src Исходное число float
 * @param dst Указатель на decimal для сохранения результата
 * @return Код ошибки (S21_CONV_OK или S21_CONV_ERR)
 */
int s21_from_float_to_decimal(float src, s21_decimal *dst) {
    if (dst == NULL) return S21_CONV_ERR;
    
    int error = S21_CONV_OK;
    *dst = s21_decimal_zero_local();

    error = s21_can_convert_float_to_decimal(src);
    if (!error) error = s21_convert_float_to_decimal_with_precision(src, dst);

    return error;
}

/**
 * @brief Преобразует decimal в целое число int
 * @details Вызывающая сторона должна обеспечить валидность указателя dst и decimal.
 *          Функция выполняет проверку переполнения и усечение дробной части.
 * @param src Исходное decimal число
 * @param dst Указатель на int для сохранения результата
 * @return Код ошибки (S21_CONV_OK или S21_CONV_ERR)
 */
int s21_from_decimal_to_int(s21_decimal src, int *dst) {
    if (dst == NULL || s21_validate_unused_bits(&src)) return S21_CONV_ERR;

    int error = S21_CONV_OK;
    s21_decimal truncated;

    error = s21_truncate(src, &truncated);
    if (!error) error = s21_can_convert_decimal_to_int(truncated);
    if (!error) *dst = s21_convert_decimal_to_int(truncated);

    return error;
}

/**
 * @brief Преобразует decimal в число float
 * @details Вызывающая сторона должна обеспечить валидность указателя dst.
 *          Функция выполняет преобразование с учетом масштаба decimal.
 * @param src Исходное decimal число
 * @param dst Указатель на float для сохранения результата
 * @return Код ошибки (S21_CONV_OK или S21_CONV_ERR)
 */
int s21_from_decimal_to_float(s21_decimal src, float *dst) {
    if (dst == NULL) return S21_CONV_ERR;
    
    *dst = 0.0f;
    
    // Проверка на нулевое значение
    if (s21_is_zero(src)) {
        return S21_CONV_OK;
    }
    
    // Извлечение знака, масштаба и мантиссы
    int sign = s21_get_sign(&src);
    int scale = s21_get_scale(&src);
    long double result = s21_decimal_to_long_double(src);
    
    // Применение масштаба
    for (int i = 0; i < scale; i++) {
        result /= 10.0L;
    }
    
    // Применение знака
    if (sign) {
        result = -result;
    }
    
    // Проверка на переполнение float
    if (result > FLT_MAX || result < -FLT_MAX) {
        return S21_CONV_ERR;
    }
    
    *dst = (float)result;
    return S21_CONV_OK;
}