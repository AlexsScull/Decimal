#include <check.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>

#include "../decimal/helpers/helpers.h"

START_TEST(test_from_int_to_decimal_basic) {
    s21_decimal result;
    int test_cases[] = {0, 1, -1, 123, -456, 7890, -12345, INT_MAX, INT_MIN};
    int expected[] = {0, 1, -1, 123, -456, 7890, -12345, INT_MAX, INT_MIN};
    
    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        int ret = s21_from_int_to_decimal(test_cases[i], &result);
        ck_assert_int_eq(ret, S21_CONV_OK);
        
        int back_converted;
        s21_from_decimal_to_int(result, &back_converted);
        ck_assert_int_eq(back_converted, expected[i]);
    }
}
END_TEST

START_TEST(test_from_int_to_decimal_null_ptr) {
    int ret = s21_from_int_to_decimal(123, NULL);
    ck_assert_int_eq(ret, S21_CONV_ERR);
}
END_TEST

START_TEST(test_from_decimal_to_int_basic) {
    s21_decimal decimals[5];
    int expected[] = {0, 123, -456, 7890, -12345};
    
    s21_from_int_to_decimal(0, &decimals[0]);
    s21_from_int_to_decimal(123, &decimals[1]);
    s21_from_int_to_decimal(-456, &decimals[2]);
    s21_from_int_to_decimal(7890, &decimals[3]);
    s21_from_int_to_decimal(-12345, &decimals[4]);
    
    for (int i = 0; i < 5; i++) {
        int result;
        int ret = s21_from_decimal_to_int(decimals[i], &result);
        ck_assert_int_eq(ret, S21_CONV_OK);
        ck_assert_int_eq(result, expected[i]);
    }
}
END_TEST

START_TEST(test_from_decimal_to_int_overflow_positive) {
    s21_decimal large_decimal;
    // Создаем decimal значение больше INT_MAX
    large_decimal.bits[0] = (uint32_t)INT_MAX + 1;
    large_decimal.bits[1] = 0;
    large_decimal.bits[2] = 0;
    large_decimal.bits[3] = 0; // scale = 0
    
    int result;
    int ret = s21_from_decimal_to_int(large_decimal, &result);
    ck_assert_int_eq(ret, S21_CONV_ERR);
}
END_TEST

START_TEST(test_from_decimal_to_int_overflow_negative) {
    s21_decimal small_decimal;
    // Создаем decimal значение меньше INT_MIN
    small_decimal.bits[0] = (uint32_t)(-(INT_MIN + 1)) + 1;
    small_decimal.bits[1] = 0;
    small_decimal.bits[2] = 0;
    s21_set_sign(&small_decimal, 1); // отрицательное
    
    int result;
    int ret = s21_from_decimal_to_int(small_decimal, &result);
    ck_assert_int_eq(ret, S21_CONV_ERR);
}
END_TEST

START_TEST(test_from_decimal_to_int_with_scale) {
    s21_decimal decimal;
    // 12.34 с scale = 2
    decimal.bits[0] = 1234;
    decimal.bits[1] = 0;
    decimal.bits[2] = 0;
    s21_set_scale(&decimal, 2);
    
    int result;
    int ret = s21_from_decimal_to_int(decimal, &result);
    ck_assert_int_eq(ret, S21_CONV_OK);
    ck_assert_int_eq(result, 12); // truncate до 12
}
END_TEST

START_TEST(test_from_decimal_to_int_null_ptr) {
    s21_decimal decimal;
    s21_from_int_to_decimal(123, &decimal);
    
    int ret = s21_from_decimal_to_int(decimal, NULL);
    ck_assert_int_eq(ret, S21_CONV_ERR);
}
END_TEST

START_TEST(test_from_float_to_decimal_basic) {
    s21_decimal result;
    float test_cases[] = {0.0f, 1.0f, -1.0f, 123.456f, -78.9f, 1000.0f, -0.001f};
    
    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        int ret = s21_from_float_to_decimal(test_cases[i], &result);
        ck_assert_int_eq(ret, S21_CONV_OK);
        
        // Проверяем что преобразование корректно через обратное преобразование
        float back_converted;
        s21_from_decimal_to_float(result, &back_converted);
        
        // Допускаем небольшую погрешность для float
        float tolerance = fabsf(test_cases[i] * 1e-6f);
        if (tolerance < 1e-7f) tolerance = 1e-7f;
        
        ck_assert_float_eq_tol(back_converted, test_cases[i], tolerance);
    }
}
END_TEST

START_TEST(test_from_float_to_decimal_special_values) {
    s21_decimal result;
    
    // NaN должен возвращать ошибку
    int ret = s21_from_float_to_decimal(NAN, &result);
    ck_assert_int_eq(ret, S21_CONV_ERR);
    
    // Бесконечности должны возвращать ошибку
    ret = s21_from_float_to_decimal(INFINITY, &result);
    ck_assert_int_eq(ret, S21_CONV_ERR);
    
    ret = s21_from_float_to_decimal(-INFINITY, &result);
    ck_assert_int_eq(ret, S21_CONV_ERR);
}
END_TEST

START_TEST(test_from_float_to_decimal_boundary_values) {
    s21_decimal result;
    
    // Слишком маленькое число
    int ret = s21_from_float_to_decimal(1e-29f, &result);
    ck_assert_int_eq(ret, S21_CONV_ERR);
    
    // Слишком большое число
    ret = s21_from_float_to_decimal(8e28f, &result);
    ck_assert_int_eq(ret, S21_CONV_ERR);
    
    // Граничное значение (должно работать)
    ret = s21_from_float_to_decimal(7.9e28f, &result);
    ck_assert_int_eq(ret, S21_CONV_OK);
    
    // Граничное значение (должно работать)
    ret = s21_from_float_to_decimal(1e-28f, &result);
    ck_assert_int_eq(ret, S21_CONV_OK);
}
END_TEST

START_TEST(test_from_float_to_decimal_precision) {
    s21_decimal result;
    
    // Число с более чем 7 значащими цифрами должно округляться
    float precise_float = 123.456789f; // 9 значащих цифр
    
    int ret = s21_from_float_to_decimal(precise_float, &result);
    ck_assert_int_eq(ret, S21_CONV_OK);
    
    float back_converted;
    s21_from_decimal_to_float(result, &back_converted);
    
    // Проверяем что округление произошло до 7 значащих цифр
    ck_assert_float_eq_tol(back_converted, 123.4568f, 1e-4f);
}
END_TEST

START_TEST(test_from_float_to_decimal_null_ptr) {
    int ret = s21_from_float_to_decimal(123.456f, NULL);
    ck_assert_int_eq(ret, S21_CONV_ERR);
}
END_TEST

START_TEST(test_from_decimal_to_float_basic) {
    s21_decimal decimals[6];
    float expected[] = {0.0f, 123.0f, -456.0f, 78.9f, -0.123f, 1000.0f};
    
    s21_from_float_to_decimal(0.0f, &decimals[0]);
    s21_from_float_to_decimal(123.0f, &decimals[1]);
    s21_from_float_to_decimal(-456.0f, &decimals[2]);
    s21_from_float_to_decimal(78.9f, &decimals[3]);
    s21_from_float_to_decimal(-0.123f, &decimals[4]);
    s21_from_float_to_decimal(1000.0f, &decimals[5]);
    
    for (int i = 0; i < 6; i++) {
        float result;
        int ret = s21_from_decimal_to_float(decimals[i], &result);
        ck_assert_int_eq(ret, S21_CONV_OK);
        
        float tolerance = fabsf(expected[i] * 1e-6f);
        if (tolerance < 1e-7f) tolerance = 1e-7f;
        
        ck_assert_float_eq_tol(result, expected[i], tolerance);
    }
}
END_TEST

START_TEST(test_from_decimal_to_float_with_scale) {
    s21_decimal decimal;
    
    // 123.456 с scale = 3
    decimal.bits[0] = 123456;
    decimal.bits[1] = 0;
    decimal.bits[2] = 0;
    s21_set_scale(&decimal, 3);
    
    float result;
    int ret = s21_from_decimal_to_float(decimal, &result);
    ck_assert_int_eq(ret, S21_CONV_OK);
    ck_assert_float_eq_tol(result, 123.456f, 1e-6f);
}
END_TEST

START_TEST(test_from_decimal_to_float_large_number) {
    s21_decimal decimal;
    
    // Большое число
    decimal.bits[0] = 1234567890;
    decimal.bits[1] = 0;
    decimal.bits[2] = 0;
    s21_set_scale(&decimal, 0);
    
    float result;
    int ret = s21_from_decimal_to_float(decimal, &result);
    ck_assert_int_eq(ret, S21_CONV_OK);
    ck_assert_float_eq_tol(result, 1234567890.0f, 1e-6f);
}
END_TEST

START_TEST(test_from_decimal_to_float_overflow) {
    s21_decimal huge_decimal;
    
    // Создаем decimal значение больше FLT_MAX
    huge_decimal.bits[0] = UINT32_MAX;
    huge_decimal.bits[1] = UINT32_MAX;
    huge_decimal.bits[2] = UINT32_MAX;
    s21_set_scale(&huge_decimal, 0);
    
    float result;
    int ret = s21_from_decimal_to_float(huge_decimal, &result);
    ck_assert_int_eq(ret, S21_CONV_ERR);
}
END_TEST

START_TEST(test_from_decimal_to_float_null_ptr) {
    s21_decimal decimal;
    s21_from_float_to_decimal(123.456f, &decimal);
    
    int ret = s21_from_decimal_to_float(decimal, NULL);
    ck_assert_int_eq(ret, S21_CONV_ERR);
}
END_TEST

START_TEST(test_round_trip_conversion) {
    // Тест циклического преобразования int -> decimal -> int
    for (int i = -1000; i <= 1000; i += 100) {
        s21_decimal decimal;
        int ret1 = s21_from_int_to_decimal(i, &decimal);
        ck_assert_int_eq(ret1, S21_CONV_OK);
        
        int result;
        int ret2 = s21_from_decimal_to_int(decimal, &result);
        ck_assert_int_eq(ret2, S21_CONV_OK);
        ck_assert_int_eq(result, i);
    }
}
END_TEST

START_TEST(test_round_trip_float_conversion) {
    // Тест циклического преобразования float -> decimal -> float
    float test_values[] = {0.5f, -0.5f, 3.14159f, -2.71828f, 100.0f, -100.0f, 0.001f, -0.001f};
    
    for (size_t i = 0; i < sizeof(test_values)/sizeof(test_values[0]); i++) {
        s21_decimal decimal;
        int ret1 = s21_from_float_to_decimal(test_values[i], &decimal);
        ck_assert_int_eq(ret1, S21_CONV_OK);
        
        float result;
        int ret2 = s21_from_decimal_to_float(decimal, &result);
        ck_assert_int_eq(ret2, S21_CONV_OK);
        
        float tolerance = fabsf(test_values[i] * 1e-6f);
        if (tolerance < 1e-7f) tolerance = 1e-7f;
        
        ck_assert_float_eq_tol(result, test_values[i], tolerance);
    }
}
END_TEST

Suite *conversion_suite(void) {
    Suite *s;
    TCase *tc_core;
    TCase *tc_boundary;
    TCase *tc_errors;

    s = suite_create("Decimal Conversion");

    /* Основные тестовые случаи */
    tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_from_int_to_decimal_basic);
    tcase_add_test(tc_core, test_from_decimal_to_int_basic);
    tcase_add_test(tc_core, test_from_float_to_decimal_basic);
    tcase_add_test(tc_core, test_from_decimal_to_float_basic);
    tcase_add_test(tc_core, test_round_trip_conversion);
    tcase_add_test(tc_core, test_round_trip_float_conversion);
    suite_add_tcase(s, tc_core);

    /* Граничные значения и особые случаи */
    tc_boundary = tcase_create("Boundary");
    tcase_add_test(tc_boundary, test_from_decimal_to_int_overflow_positive);
    tcase_add_test(tc_boundary, test_from_decimal_to_int_overflow_negative);
    tcase_add_test(tc_boundary, test_from_decimal_to_int_with_scale);
    tcase_add_test(tc_boundary, test_from_float_to_decimal_special_values);
    tcase_add_test(tc_boundary, test_from_float_to_decimal_boundary_values);
    tcase_add_test(tc_boundary, test_from_float_to_decimal_precision);
    tcase_add_test(tc_boundary, test_from_decimal_to_float_with_scale);
    tcase_add_test(tc_boundary, test_from_decimal_to_float_large_number);
    tcase_add_test(tc_boundary, test_from_decimal_to_float_overflow);
    suite_add_tcase(s, tc_boundary);

    /* Обработка ошибок */
    tc_errors = tcase_create("Errors");
    tcase_add_test(tc_errors, test_from_int_to_decimal_null_ptr);
    tcase_add_test(tc_errors, test_from_decimal_to_int_null_ptr);
    tcase_add_test(tc_errors, test_from_float_to_decimal_null_ptr);
    tcase_add_test(tc_errors, test_from_decimal_to_float_null_ptr);
    suite_add_tcase(s, tc_errors);

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
  Suite *s = conversion_suite();
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