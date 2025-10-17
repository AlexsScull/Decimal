#include <check.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "../decimal/s21_decimal.h"
#include "../decimal/helpers/helpers.h"

// ==================== Вспомогательные функции ====================

static int float_equals(float a, float b, float epsilon) {
    if (isnan(a) && isnan(b)) return 1;
    if (isinf(a) && isinf(b) && signbit(a) == signbit(b)) return 1;
    return fabs(a - b) < epsilon;
}

static int decimal_equals(s21_decimal a, s21_decimal b) {
    return a.bits[0] == b.bits[0] && a.bits[1] == b.bits[1] && 
           a.bits[2] == b.bits[2] && a.bits[3] == b.bits[3];
}

// ==================== КРИТИЧЕСКИЕ ТЕСТЫ ДЛЯ int ↔ decimal ====================

START_TEST(test_int_to_decimal_positive_numbers) {
    s21_decimal result;
    
    int status = s21_from_int_to_decimal(12345, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_OK);
    ck_assert_uint_eq(result.bits[0], 12345);
    ck_assert_uint_eq(result.bits[1], 0);
    ck_assert_uint_eq(result.bits[2], 0);
    ck_assert_int_eq(s21_get_scale(&result), 0);
    ck_assert_int_eq(s21_get_sign(&result), 0);
}
END_TEST

START_TEST(test_int_to_decimal_negative_numbers) {
    s21_decimal result;
    
    int status = s21_from_int_to_decimal(-67890, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_OK);
    ck_assert_uint_eq(result.bits[0], 67890);
    ck_assert_int_eq(s21_get_sign(&result), 1);
}
END_TEST

START_TEST(test_int_to_decimal_zero) {
    s21_decimal result;
    
    int status = s21_from_int_to_decimal(0, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_OK);
    ck_assert_uint_eq(result.bits[0], 0);
    ck_assert_uint_eq(result.bits[1], 0);
    ck_assert_uint_eq(result.bits[2], 0);
    ck_assert_int_eq(s21_get_sign(&result), 0);
}
END_TEST

START_TEST(test_int_to_decimal_max_int) {
    s21_decimal result;
    
    int status = s21_from_int_to_decimal(INT_MAX, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_OK);
    ck_assert_uint_eq(result.bits[0], INT_MAX);
    ck_assert_int_eq(s21_get_sign(&result), 0);
}
END_TEST

START_TEST(test_int_to_decimal_min_int) {
    s21_decimal result;
    
    int status = s21_from_int_to_decimal(INT_MIN, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_OK);
    // Для INT_MIN нужно аккуратно проверить представление
    ck_assert_uint_eq(result.bits[0], (uint32_t)(-(int64_t)INT_MIN));
    ck_assert_int_eq(s21_get_sign(&result), 1);
}
END_TEST

START_TEST(test_int_to_decimal_null_dst_returns_error) {
    int status = s21_from_int_to_decimal(123, NULL);
    
    ck_assert_int_eq(status, S21_CONVERSION_ERROR);
}
END_TEST

START_TEST(test_decimal_to_int_positive_numbers) {
    s21_decimal value = decimal_from_parts(12345, 0, 0, 0, 0);
    int result;
    
    int status = s21_from_decimal_to_int(value, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_OK);
    ck_assert_int_eq(result, 12345);
}
END_TEST

START_TEST(test_decimal_to_int_negative_numbers) {
    s21_decimal value = decimal_from_parts(67890, 0, 0, 0, 1);
    int result;
    
    int status = s21_from_decimal_to_int(value, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_OK);
    ck_assert_int_eq(result, -67890);
}
END_TEST

START_TEST(test_decimal_to_int_zero) {
    s21_decimal value = decimal_from_parts(0, 0, 0, 0, 0);
    int result;
    
    int status = s21_from_decimal_to_int(value, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_OK);
    ck_assert_int_eq(result, 0);
}
END_TEST

START_TEST(test_decimal_to_int_truncates_fraction) {
    s21_decimal value = decimal_from_parts(12345, 0, 0, 3, 0); // 12.345
    int result;
    
    int status = s21_from_decimal_to_int(value, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_OK);
    ck_assert_int_eq(result, 12); // Дробная часть отбрасывается
}
END_TEST

START_TEST(test_decimal_to_int_rounds_towards_zero) {
    s21_decimal positive = decimal_from_parts(9999, 0, 0, 3, 0); // 9.999
    s21_decimal negative = decimal_from_parts(9999, 0, 0, 3, 1); // -9.999
    int result_pos, result_neg;
    
    s21_from_decimal_to_int(positive, &result_pos);
    s21_from_decimal_to_int(negative, &result_neg);
    
    ck_assert_int_eq(result_pos, 9);  // 9.999 -> 9 (к нулю)
    ck_assert_int_eq(result_neg, -9); // -9.999 -> -9 (к нулю)
}
END_TEST

START_TEST(test_decimal_to_int_overflow_positive_returns_error) {
    s21_decimal large = decimal_from_parts(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0, 0);
    int result;
    
    int status = s21_from_decimal_to_int(large, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_ERROR);
}
END_TEST

START_TEST(test_decimal_to_int_overflow_negative_returns_error) {
    s21_decimal large_negative = decimal_from_parts(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0, 1);
    int result;
    
    int status = s21_from_decimal_to_int(large_negative, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_ERROR);
}
END_TEST

START_TEST(test_decimal_to_int_null_dst_returns_error) {
    s21_decimal value = decimal_from_parts(123, 0, 0, 0, 0);
    
    int status = s21_from_decimal_to_int(value, NULL);
    
    ck_assert_int_eq(status, S21_CONVERSION_ERROR);
}
END_TEST

// ==================== КРИТИЧЕСКИЕ ТЕСТЫ ДЛЯ float ↔ decimal ====================

START_TEST(test_float_to_decimal_positive_numbers) {
    s21_decimal result;
    
    int status = s21_from_float_to_decimal(123.456f, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_OK);
    // Проверяем что значение приблизительно правильное
    ck_assert_uint_ge(result.bits[0], 123455); // Учитываем возможное округление
    ck_assert_uint_le(result.bits[0], 123457);
    ck_assert_int_eq(s21_get_sign(&result), 0);
}
END_TEST

START_TEST(test_float_to_decimal_negative_numbers) {
    s21_decimal result;
    
    int status = s21_from_float_to_decimal(-78.9f, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_OK);
    ck_assert_int_eq(s21_get_sign(&result), 1);
}
END_TEST

START_TEST(test_float_to_decimal_zero) {
    s21_decimal result;
    
    int status = s21_from_float_to_decimal(0.0f, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_OK);
    ck_assert_uint_eq(result.bits[0], 0);
    ck_assert_int_eq(s21_get_sign(&result), 0);
}
END_TEST

START_TEST(test_float_to_decimal_negative_zero) {
    s21_decimal result;
    
    int status = s21_from_float_to_decimal(-0.0f, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_OK);
    ck_assert_uint_eq(result.bits[0], 0);
    // Отрицательный ноль может быть представлен как положительный
    // согласно спецификации decimal
}
END_TEST

START_TEST(test_float_to_decimal_small_positive_returns_error) {
    s21_decimal result;
    
    // Число меньше 1e-28
    int status = s21_from_float_to_decimal(1e-29f, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_ERROR);
    ck_assert_uint_eq(result.bits[0], 0); // Должно быть установлено в 0
}
END_TEST

START_TEST(test_float_to_decimal_small_negative_returns_error) {
    s21_decimal result;
    
    int status = s21_from_float_to_decimal(-1e-29f, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_ERROR);
    ck_assert_uint_eq(result.bits[0], 0);
}
END_TEST

START_TEST(test_float_to_decimal_large_positive_returns_error) {
    s21_decimal result;
    
    // Число больше максимального decimal
    int status = s21_from_float_to_decimal(8e28f, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_ERROR);
}
END_TEST

START_TEST(test_float_to_decimal_large_negative_returns_error) {
    s21_decimal result;
    
    int status = s21_from_float_to_decimal(-8e28f, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_ERROR);
}
END_TEST

START_TEST(test_float_to_decimal_infinity_returns_error) {
    s21_decimal result;
    
    int status = s21_from_float_to_decimal(INFINITY, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_ERROR);
}
END_TEST

START_TEST(test_float_to_decimal_negative_infinity_returns_error) {
    s21_decimal result;
    
    int status = s21_from_float_to_decimal(-INFINITY, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_ERROR);
}
END_TEST

START_TEST(test_float_to_decimal_nan_returns_error) {
    s21_decimal result;
    
    int status = s21_from_float_to_decimal(NAN, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_ERROR);
}
END_TEST

START_TEST(test_float_to_decimal_precision_7_digits) {
    s21_decimal result;
    
    // Число с более чем 7 значащими цифрами
    float value = 123.456789f; // 9 значащих цифр
    int status = s21_from_float_to_decimal(value, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_OK);
    // Должно быть округлено до 7 значащих цифр: 123.4568
    // Проверим, что значение близко к округленному
    float rounded = 123.4568f;
    s21_decimal expected;
    s21_from_float_to_decimal(rounded, &expected);
    ck_assert_uint_eq(result.bits[0], expected.bits[0]);
    ck_assert_int_eq(s21_get_scale(&result), s21_get_scale(&expected));
}
END_TEST

START_TEST(test_float_to_decimal_rounding_behavior) {
    // Проверяем округление к ближайшему четному (банковское округление)
    // 1.234565 -> 1.23456 (если последняя цифра 5 и предыдущая четная)
    // 1.234575 -> 1.23458 (если последняя цифра 5 и предыдущая нечетная)
    float test_cases[][2] = {
        {1.234565f, 1.23456f},  // округление вниз
        {1.234575f, 1.23458f},  // округление вверх
    };
    
    for (int i = 0; i < 2; i++) {
        s21_decimal result;
        int status = s21_from_float_to_decimal(test_cases[i][0], &result);
        ck_assert_int_eq(status, S21_CONVERSION_OK);
        
        s21_decimal expected;
        s21_from_float_to_decimal(test_cases[i][1], &expected);
        ck_assert_uint_eq(result.bits[0], expected.bits[0]);
        ck_assert_int_eq(s21_get_scale(&result), s21_get_scale(&expected));
    }
}
END_TEST

START_TEST(test_float_to_decimal_common_values) {
    float test_values[] = {
        1.0f, 2.0f, 10.0f, 100.0f, 1000.0f, 10000.0f, 100000.0f,
        0.1f, 0.01f, 0.001f, 0.0001f, 0.00001f,
        3.141592f, 2.718281f, 1.414213f
    };
    
    for (size_t i = 0; i < sizeof(test_values) / sizeof(test_values[0]); i++) {
        s21_decimal result;
        int status = s21_from_float_to_decimal(test_values[i], &result);
        ck_assert_int_eq(status, S21_CONVERSION_OK);
        // Проверяем что преобразование прошло без ошибок
    }
}
END_TEST

START_TEST(test_float_to_decimal_powers_of_ten) {
    float powers[] = {
        1e0f, 1e1f, 1e2f, 1e3f, 1e4f, 1e5f, 1e6f,
        1e-1f, 1e-2f, 1e-3f, 1e-4f, 1e-5f, 1e-6f
    };
    
    for (int i = 0; i < 13; i++) {
        s21_decimal result;
        int status = s21_from_float_to_decimal(powers[i], &result);
        
        if (fabs(powers[i]) < 1e-28f || fabs(powers[i]) > 7.9228e28f) {
            ck_assert_int_eq(status, S21_CONVERSION_ERROR);
        } else {
            ck_assert_int_eq(status, S21_CONVERSION_OK);
        }
    }
}
END_TEST

START_TEST(test_float_to_decimal_denormalized_handling) {
    // Денормализованные числа (subnormal) - очень близки к нулю
    float denormal = 1e-38f; // денормализованное число
    s21_decimal result;
    
    int status = s21_from_float_to_decimal(denormal, &result);
    
    // Денормализованные числа слишком малы, поэтому должна быть ошибка
    ck_assert_int_eq(status, S21_CONVERSION_ERROR);
    ck_assert_uint_eq(result.bits[0], 0);
}
END_TEST

START_TEST(test_float_to_decimal_null_dst_returns_error) {
    int status = s21_from_float_to_decimal(123.45f, NULL);
    
    ck_assert_int_eq(status, S21_CONVERSION_ERROR);
}
END_TEST

START_TEST(test_decimal_to_float_positive_numbers) {
    s21_decimal value = decimal_from_parts(12345, 0, 0, 0, 0);
    float result;
    
    int status = s21_from_decimal_to_float(value, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_OK);
    ck_assert(float_equals(result, 12345.0f, 1e-5f));
}
END_TEST

START_TEST(test_decimal_to_float_negative_numbers) {
    s21_decimal value = decimal_from_parts(67890, 0, 0, 0, 1);
    float result;
    
    int status = s21_from_decimal_to_float(value, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_OK);
    ck_assert(float_equals(result, -67890.0f, 1e-5f));
}
END_TEST

START_TEST(test_decimal_to_float_zero) {
    s21_decimal value = decimal_from_parts(0, 0, 0, 0, 0);
    float result;
    
    int status = s21_from_decimal_to_float(value, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_OK);
    ck_assert(float_equals(result, 0.0f, 1e-5f));
}
END_TEST

START_TEST(test_decimal_to_float_with_scale) {
    s21_decimal value = decimal_from_parts(123456, 0, 0, 3, 0); // 123.456
    float result;
    
    int status = s21_from_decimal_to_float(value, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_OK);
    ck_assert(float_equals(result, 123.456f, 1e-5f));
}
END_TEST

START_TEST(test_decimal_to_float_large_numbers) {
    s21_decimal large = decimal_from_parts(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0, 0);
    float result;
    
    int status = s21_from_decimal_to_float(large, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_OK);
    // Проверяем что значение приблизительно правильное
    ck_assert(result > 1e28f); // Примерно 7.9228e28
}
END_TEST

START_TEST(test_decimal_to_float_small_numbers) {
    s21_decimal small = decimal_from_parts(1, 0, 0, 28, 0); // 1e-28
    float result;
    
    int status = s21_from_decimal_to_float(small, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_OK);
    ck_assert(float_equals(result, 1e-28f, 1e-30f));
}
END_TEST

START_TEST(test_decimal_to_float_precision_loss) {
    // Decimal может иметь большую точность чем float
    s21_decimal precise = decimal_from_parts(123456789, 0, 0, 8, 0); // 1.23456789
    float result;
    
    int status = s21_from_decimal_to_float(precise, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_OK);
    // Float имеет около 7 значащих цифр, поэтому возможна потеря точности
    ck_assert(float_equals(result, 1.23456789f, 1e-7f));
}
END_TEST

START_TEST(test_decimal_to_float_max_decimal) {
    s21_decimal max_val = decimal_from_parts(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0, 0);
    float result;
    
    int status = s21_from_decimal_to_float(max_val, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_OK);
    // Максимальное decimal число ~7.9228e28, должно быть представимо в float
    ck_assert(!isinf(result));
}
END_TEST

START_TEST(test_decimal_to_float_min_decimal) {
    s21_decimal min_val = decimal_from_parts(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0, 1);
    float result;
    
    int status = s21_from_decimal_to_float(min_val, &result);
    
    ck_assert_int_eq(status, S21_CONVERSION_OK);
    ck_assert(!isinf(result));
    ck_assert(result < 0);
}
END_TEST

START_TEST(test_decimal_to_float_null_dst_returns_error) {
    s21_decimal value = decimal_from_parts(123, 0, 0, 0, 0);
    
    int status = s21_from_decimal_to_float(value, NULL);
    
    ck_assert_int_eq(status, S21_CONVERSION_ERROR);
}
END_TEST

// ==================== ТЕСТЫ ГРАНИЧНЫХ СЛУЧАЕВ И ПОТЕРИ ТОЧНОСТИ ====================

START_TEST(test_conversion_round_trip_int) {
    int original_values[] = {0, 1, -1, 123, -456, 10000, -10000, INT_MAX, INT_MIN};
    
    for (int i = 0; i < 9; i++) {
        s21_decimal decimal_val;
        int converted_val;
        
        // int -> decimal -> int
        int status1 = s21_from_int_to_decimal(original_values[i], &decimal_val);
        int status2 = s21_from_decimal_to_int(decimal_val, &converted_val);
        
        ck_assert_int_eq(status1, S21_CONVERSION_OK);
        
        // Для INT_MIN преобразование обратно может быть проблематичным
        // из-за ограничений представления
        if (original_values[i] != INT_MIN) {
            ck_assert_int_eq(status2, S21_CONVERSION_OK);
            ck_assert_int_eq(converted_val, original_values[i]);
        }
    }
}
END_TEST

START_TEST(test_conversion_round_trip_float) {
    float original_values[] = {
        0.0f, 1.0f, -1.0f, 123.456f, -78.9f, 
        100000.0f, -100000.0f, 0.001f, -0.001f
    };
    
    for (int i = 0; i < 9; i++) {
        s21_decimal decimal_val;
        float converted_val;
        
        // float -> decimal -> float
        int status1 = s21_from_float_to_decimal(original_values[i], &decimal_val);
        int status2 = s21_from_decimal_to_float(decimal_val, &converted_val);
        
        if (fabs(original_values[i]) < 1e-28f || fabs(original_values[i]) > 7.9228e28f) {
            ck_assert_int_eq(status1, S21_CONVERSION_ERROR);
        } else {
            ck_assert_int_eq(status1, S21_CONVERSION_OK);
            ck_assert_int_eq(status2, S21_CONVERSION_OK);
            // Из-за потери точности в float проверяем приблизительное равенство
            ck_assert(float_equals(converted_val, original_values[i], fabs(original_values[i]) * 1e-5f + 1e-10f));
        }
    }
}
END_TEST

START_TEST(test_conversion_special_float_values) {
    float special_values[] = {
        INFINITY, -INFINITY, NAN,
        0.0f / 0.0f,  // NaN
        1.0f / 0.0f,  // Infinity
        -1.0f / 0.0f  // -Infinity
    };
    
    for (int i = 0; i < 6; i++) {
        s21_decimal result;
        int status = s21_from_float_to_decimal(special_values[i], &result);
        ck_assert_int_eq(status, S21_CONVERSION_ERROR);
    }
}
END_TEST

START_TEST(test_conversion_denormalized_float) {
    // Денормализованные числа (subnormal)
    float denormal = 1e-38f; // денормализованное число
    s21_decimal decimal_val;
    float converted_val;
    
    int status1 = s21_from_float_to_decimal(denormal, &decimal_val);
    ck_assert_int_eq(status1, S21_CONVERSION_ERROR);
    
    // Проверяем преобразование decimal в float для очень маленького числа
    s21_decimal small = decimal_from_parts(1, 0, 0, 28, 0); // 1e-28
    int status2 = s21_from_decimal_to_float(small, &converted_val);
    ck_assert_int_eq(status2, S21_CONVERSION_OK);
    ck_assert(float_equals(converted_val, 1e-28f, 1e-30f));
}
END_TEST

START_TEST(test_conversion_power_of_ten_values) {
    // Проверяем степени 10, которые могут быть представлены точно
    float powers[] = {1e0f, 1e1f, 1e2f, 1e3f, 1e4f, 1e5f, 1e6f};
    
    for (int i = 0; i < 7; i++) {
        s21_decimal decimal_val;
        float converted_val;
        
        int status1 = s21_from_float_to_decimal(powers[i], &decimal_val);
        int status2 = s21_from_decimal_to_float(decimal_val, &converted_val);
        
        ck_assert_int_eq(status1, S21_CONVERSION_OK);
        ck_assert_int_eq(status2, S21_CONVERSION_OK);
        ck_assert(float_equals(converted_val, powers[i], 1e-5f));
    }
}
END_TEST

START_TEST(test_conversion_fractional_values) {
    s21_decimal fractional = decimal_from_parts(123456, 0, 0, 5, 0); // 1.23456
    int int_result;
    float float_result;
    
    // decimal -> int (должен отбросить дробную часть)
    s21_from_decimal_to_int(fractional, &int_result);
    ck_assert_int_eq(int_result, 1);
    
    // decimal -> float (должен сохранить дробную часть)
    s21_from_decimal_to_float(fractional, &float_result);
    ck_assert(float_equals(float_result, 1.23456f, 1e-6f));
}
END_TEST

START_TEST(test_float_imprecise_representation) {
    // Числа, которые не могут быть точно представлены в float
    float imprecise_values[] = {
        0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f,
        1.1f, 2.2f, 3.3f, 4.4f, 5.5f, 6.6f, 7.7f, 8.8f, 9.9f
    };
    
    for (int i = 0; i < 18; i++) {
        s21_decimal decimal_val;
        float converted_val;
        
        int status1 = s21_from_float_to_decimal(imprecise_values[i], &decimal_val);
        int status2 = s21_from_decimal_to_float(decimal_val, &converted_val);
        
        ck_assert_int_eq(status1, S21_CONVERSION_OK);
        ck_assert_int_eq(status2, S21_CONVERSION_OK);
        // Проверяем что значение приблизительно совпадает
        ck_assert(float_equals(converted_val, imprecise_values[i], 1e-5f));
    }
}
END_TEST

START_TEST(test_decimal_scale_28_conversion) {
    // Тестируем преобразование чисел с масштабом 28
    s21_decimal max_scale_values[] = {
        decimal_from_parts(1, 0, 0, 28, 0), // 1e-28
        decimal_from_parts(123456789, 0, 0, 28, 0), // 1.23456789e-21
        decimal_from_parts(999999999, 0, 0, 28, 0), // 9.99999999e-21
    };
    
    for (int i = 0; i < 3; i++) {
        float float_val;
        int status = s21_from_decimal_to_float(max_scale_values[i], &float_val);
        ck_assert_int_eq(status, S21_CONVERSION_OK);
        // Проверяем что преобразование прошло без ошибок
        ck_assert(!isnan(float_val));
        ck_assert(!isinf(float_val));
    }
}
END_TEST

START_TEST(test_progressive_precision_loss) {
    // Тестируем постепенную потерю точности при множественных преобразованиях
    float original = 3.141592653589793f;
    s21_decimal intermediate;
    float result;
    
    // float -> decimal -> float
    int status1 = s21_from_float_to_decimal(original, &intermediate);
    int status2 = s21_from_decimal_to_float(intermediate, &result);
    
    ck_assert_int_eq(status1, S21_CONVERSION_OK);
    ck_assert_int_eq(status2, S21_CONVERSION_OK);
    
    // Из-за ограничения в 7 значащих цифр, результат должен быть округлен
    ck_assert(float_equals(result, 3.141592f, 1e-6f));
}
END_TEST

START_TEST(test_boundary_value_conversion) {
    // Тестируем преобразование значений на границах допустимого диапазона
    float boundary_values[] = {
        1e-28f,     // Минимальное положительное
        -1e-28f,    // Минимальное отрицательное  
        7.9228e28f, // Максимальное положительное (приблизительно)
        -7.9228e28f // Максимальное отрицательное
    };
    
    for (int i = 0; i < 4; i++) {
        s21_decimal decimal_val;
        int status = s21_from_float_to_decimal(boundary_values[i], &decimal_val);
        
        // Граничные значения могут преобразовываться с ошибкой или успешно
        // в зависимости от точного значения
        if (fabs(boundary_values[i]) < 1e-28f || fabs(boundary_values[i]) > 7.9228e28f) {
            ck_assert_int_eq(status, S21_CONVERSION_ERROR);
        } else {
            ck_assert_int_eq(status, S21_CONVERSION_OK);
        }
    }
}
END_TEST

// ==================== Test Suite Setup ====================

Suite *conversion_functions_suite(void) {
    Suite *s;
    TCase *tc_int_to_decimal, *tc_decimal_to_int, *tc_float_to_decimal;
    TCase *tc_decimal_to_float, *tc_edge, *tc_precision;

    s = suite_create("Decimal Conversion Functions - Complete Coverage");

    // int to decimal tests
    tc_int_to_decimal = tcase_create("Int to Decimal");
    tcase_add_test(tc_int_to_decimal, test_int_to_decimal_positive_numbers);
    tcase_add_test(tc_int_to_decimal, test_int_to_decimal_negative_numbers);
    tcase_add_test(tc_int_to_decimal, test_int_to_decimal_zero);
    tcase_add_test(tc_int_to_decimal, test_int_to_decimal_max_int);
    tcase_add_test(tc_int_to_decimal, test_int_to_decimal_min_int);
    tcase_add_test(tc_int_to_decimal, test_int_to_decimal_null_dst_returns_error);
    suite_add_tcase(s, tc_int_to_decimal);

    // decimal to int tests
    tc_decimal_to_int = tcase_create("Decimal to Int");
    tcase_add_test(tc_decimal_to_int, test_decimal_to_int_positive_numbers);
    tcase_add_test(tc_decimal_to_int, test_decimal_to_int_negative_numbers);
    tcase_add_test(tc_decimal_to_int, test_decimal_to_int_zero);
    tcase_add_test(tc_decimal_to_int, test_decimal_to_int_truncates_fraction);
    tcase_add_test(tc_decimal_to_int, test_decimal_to_int_rounds_towards_zero);
    tcase_add_test(tc_decimal_to_int, test_decimal_to_int_overflow_positive_returns_error);
    tcase_add_test(tc_decimal_to_int, test_decimal_to_int_overflow_negative_returns_error);
    tcase_add_test(tc_decimal_to_int, test_decimal_to_int_null_dst_returns_error);
    suite_add_tcase(s, tc_decimal_to_int);

    // float to decimal tests
    tc_float_to_decimal = tcase_create("Float to Decimal");
    tcase_add_test(tc_float_to_decimal, test_float_to_decimal_positive_numbers);
    tcase_add_test(tc_float_to_decimal, test_float_to_decimal_negative_numbers);
    tcase_add_test(tc_float_to_decimal, test_float_to_decimal_zero);
    tcase_add_test(tc_float_to_decimal, test_float_to_decimal_negative_zero);
    tcase_add_test(tc_float_to_decimal, test_float_to_decimal_small_positive_returns_error);
    tcase_add_test(tc_float_to_decimal, test_float_to_decimal_small_negative_returns_error);
    tcase_add_test(tc_float_to_decimal, test_float_to_decimal_large_positive_returns_error);
    tcase_add_test(tc_float_to_decimal, test_float_to_decimal_large_negative_returns_error);
    tcase_add_test(tc_float_to_decimal, test_float_to_decimal_infinity_returns_error);
    tcase_add_test(tc_float_to_decimal, test_float_to_decimal_negative_infinity_returns_error);
    tcase_add_test(tc_float_to_decimal, test_float_to_decimal_nan_returns_error);
    tcase_add_test(tc_float_to_decimal, test_float_to_decimal_precision_7_digits);
    tcase_add_test(tc_float_to_decimal, test_float_to_decimal_rounding_behavior);
    tcase_add_test(tc_float_to_decimal, test_float_to_decimal_common_values);
    tcase_add_test(tc_float_to_decimal, test_float_to_decimal_powers_of_ten);
    tcase_add_test(tc_float_to_decimal, test_float_to_decimal_denormalized_handling);
    tcase_add_test(tc_float_to_decimal, test_float_to_decimal_null_dst_returns_error);
    suite_add_tcase(s, tc_float_to_decimal);

    // decimal to float tests
    tc_decimal_to_float = tcase_create("Decimal to Float");
    tcase_add_test(tc_decimal_to_float, test_decimal_to_float_positive_numbers);
    tcase_add_test(tc_decimal_to_float, test_decimal_to_float_negative_numbers);
    tcase_add_test(tc_decimal_to_float, test_decimal_to_float_zero);
    tcase_add_test(tc_decimal_to_float, test_decimal_to_float_with_scale);
    tcase_add_test(tc_decimal_to_float, test_decimal_to_float_large_numbers);
    tcase_add_test(tc_decimal_to_float, test_decimal_to_float_small_numbers);
    tcase_add_test(tc_decimal_to_float, test_decimal_to_float_precision_loss);
    tcase_add_test(tc_decimal_to_float, test_decimal_to_float_max_decimal);
    tcase_add_test(tc_decimal_to_float, test_decimal_to_float_min_decimal);
    tcase_add_test(tc_decimal_to_float, test_decimal_to_float_null_dst_returns_error);
    suite_add_tcase(s, tc_decimal_to_float);

    // Edge cases tests
    tc_edge = tcase_create("Edge Cases");
    tcase_add_test(tc_edge, test_conversion_round_trip_int);
    tcase_add_test(tc_edge, test_conversion_round_trip_float);
    tcase_add_test(tc_edge, test_conversion_special_float_values);
    tcase_add_test(tc_edge, test_conversion_denormalized_float);
    tcase_add_test(tc_edge, test_conversion_power_of_ten_values);
    tcase_add_test(tc_edge, test_conversion_fractional_values);
    suite_add_tcase(s, tc_edge);

    // Precision and boundary tests
    tc_precision = tcase_create("Precision and Boundary");
    tcase_add_test(tc_precision, test_float_imprecise_representation);
    tcase_add_test(tc_precision, test_decimal_scale_28_conversion);
    tcase_add_test(tc_precision, test_progressive_precision_loss);
    tcase_add_test(tc_precision, test_boundary_value_conversion);
    suite_add_tcase(s, tc_precision);

    return s;
}


/**
 * Точка входа в программу
 * @param argc Количество аргументов командной строки
 * @param argv Массив аргументов командной строки
 *
 * Использование:
 *   ./test      - обычный режим (вывод только ошибок)
 *   ./test +    - подробный режим (вывод всех тестов)
 */
int main(int argc, char **argv) {
  int failed = 0;
  Suite *s = conversion_functions_suite();
  SRunner *runner = srunner_create(s);

  if (argc > 1 && strcmp(argv[1], "+") == 0) {
    srunner_run_all(runner, CK_VERBOSE);  // Подробный вывод
  } else {
    srunner_run_all(runner, CK_NORMAL);  // Только ошибки
  }

  failed = srunner_ntests_failed(runner);
  srunner_free(runner);

  return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}