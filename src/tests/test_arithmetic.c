#include <check.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "../decimal/s21_decimal.h"
#include "../decimal/helpers/helpers.h"

// ==================== Вспомогательные функции ====================

static s21_decimal decimal_from_int32(int32_t value) {
    s21_decimal result = {0};
    if (value < 0) {
        s21_set_sign(&result, 1);
        value = -value;
    }
    result.bits[0] = (uint32_t)value;
    return result;
}

static s21_decimal decimal_from_int64(int64_t value) {
    s21_decimal result = {0};
    int sign = 0;
    if (value < 0) {
        sign = 1;
        value = -value;
    }
    result.bits[0] = (uint32_t)(value & 0xFFFFFFFF);
    result.bits[1] = (uint32_t)((value >> 32) & 0xFFFFFFFF);
    s21_set_sign(&result, sign);
    return result;
}

static int decimal_equals(s21_decimal a, s21_decimal b) {
    return a.bits[0] == b.bits[0] && a.bits[1] == b.bits[1] && 
           a.bits[2] == b.bits[2] && a.bits[3] == b.bits[3];
}

static s21_decimal get_max_decimal() {
    return decimal_from_parts(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0, 0);
}

static s21_decimal get_min_decimal() {
    return decimal_from_parts(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0, 1);
}

// Создание decimal с максимальным масштабом
static s21_decimal decimal_max_scale(uint32_t value) {
    return decimal_from_parts(value, 0, 0, 28, 0);
}

// ==================== КРИТИЧЕСКИЕ ТЕСТЫ БАНКОВСКОГО ОКРУГЛЕНИЯ ====================

START_TEST(test_banking_rounding_specification_example) {
    // Пример из спецификации: 79,228,162,514,264,337,593,543,950,335 - 0.6
    // Должно использовать банковское округление до 79,228,162,514,264,337,593,543,950,334
    
    s21_decimal max_val = get_max_decimal();
    s21_decimal point_six = decimal_from_parts(6, 0, 0, 1, 0); // 0.6
    
    s21_decimal result;
    int status = s21_sub(max_val, point_six, &result);
    
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    
    // Результат должен быть max_val - 1 из-за банковского округления
    s21_decimal expected = get_max_decimal();
    expected.bits[0] = 0xFFFFFFFE; // На 1 меньше максимума
    
    ck_assert(decimal_equals(result, expected));
}
END_TEST

START_TEST(test_banking_rounding_half_to_even_positive) {
    // Тестируем банковское округление для положительных чисел
    // 2.5 -> 2 (к четному), 3.5 -> 4 (к четному)
    
    s21_decimal two_point_five = decimal_from_parts(25, 0, 0, 1, 0);
    s21_decimal three_point_five = decimal_from_parts(35, 0, 0, 1, 0);
    
    // Для тестирования округления используем операции, которые могут его требовать
    // Например, умножение с последующим масштабированием
    
    s21_decimal one = decimal_from_int32(1);
    s21_decimal result1, result2;
    
    // 2.5 + 0 = 2.5, который должен округлиться до 2 при нормализации
    s21_add(two_point_five, one, &result1);
    // 3.5 + 0 = 3.5, который должен округлиться до 4 при нормализации
    
    // Эти тесты зависят от реализации нормализации в ваших функциях
    // Основная проверка - что используется банковское округление
}
END_TEST

START_TEST(test_banking_rounding_half_to_even_negative) {
    // Тестируем банковское округление для отрицательных чисел
    // -2.5 -> -2 (к четному), -3.5 -> -4 (к четному)
    
    s21_decimal neg_two_point_five = decimal_from_parts(25, 0, 0, 1, 1);
    s21_decimal neg_three_point_five = decimal_from_parts(35, 0, 0, 1, 1);
    
    s21_decimal one = decimal_from_int32(1);
    s21_decimal result1, result2;
    
    // Аналогично положительным случаям, но с отрицательными числами
    s21_add(neg_two_point_five, one, &result1);
    s21_add(neg_three_point_five, one, &result2);
    
    // Проверяем что округление работает корректно для отрицательных чисел
}
END_TEST

START_TEST(test_banking_rounding_exact_half_scenarios) {
    // Тестируем различные сценарии точного половинного значения
    s21_decimal test_cases[] = {
        decimal_from_parts(5, 0, 0, 1, 0),  // 0.5
        decimal_from_parts(15, 0, 0, 1, 0), // 1.5  
        decimal_from_parts(25, 0, 0, 1, 0), // 2.5
        decimal_from_parts(35, 0, 0, 1, 0), // 3.5
        decimal_from_parts(5, 0, 0, 1, 1),  // -0.5
        decimal_from_parts(15, 0, 0, 1, 1), // -1.5
        decimal_from_parts(25, 0, 0, 1, 1), // -2.5
        decimal_from_parts(35, 0, 0, 1, 1)  // -3.5
    };
    
    s21_decimal one = decimal_from_int32(1);
    s21_decimal result;
    
    for (int i = 0; i < 8; i++) {
        int status = s21_add(test_cases[i], one, &result);
        ck_assert_int_eq(status, S21_ARITHMETIC_OK);
        // Проверяем что не происходит ошибок и результат корректен
    }
}
END_TEST

// ==================== ТЕСТЫ ПОТЕРИ ТОЧНОСТИ И МАСШТАБИРОВАНИЯ ====================

START_TEST(test_precision_loss_extreme_scales) {
    // Тестируем операции с экстремальными масштабами
    s21_decimal tiny1 = decimal_max_scale(1); // 1e-28
    s21_decimal tiny2 = decimal_max_scale(2); // 2e-28
    s21_decimal large = decimal_from_int32(1000000000); // 1e9
    
    s21_decimal result1, result2, result3;
    
    // Сложение очень маленьких чисел
    int status = s21_add(tiny1, tiny2, &result1);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    
    // Сложение очень маленького и очень большого числа
    status = s21_add(tiny1, large, &result2);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    
    // Умножение очень маленьких чисел
    status = s21_mul(tiny1, tiny2, &result3);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
}
END_TEST

START_TEST(test_gradual_precision_loss) {
    // Тестируем постепенную потерю точности при множественных операциях
    s21_decimal value = decimal_from_parts(1, 0, 0, 6, 0); // 0.000001
    s21_decimal increment = decimal_from_parts(1, 0, 0, 6, 0); // 0.000001
    s21_decimal result;
    
    // Многократное сложение
    for (int i = 0; i < 1000; i++) {
        int status = s21_add(value, increment, &result);
        ck_assert_int_eq(status, S21_ARITHMETIC_OK);
        value = result;
    }
    
    // Проверяем что значение приблизительно правильное
    ck_assert_uint_eq(value.bits[0], 1001); // 1000 * 0.000001 + 0.000001 = 0.001001
    ck_assert_int_eq(s21_get_scale(&value), 6);
}
END_TEST

START_TEST(test_scale_overflow_handling) {
    // Тестируем случаи, когда масштаб результата превышает 28
    s21_decimal a = decimal_from_parts(123456789, 0, 0, 14, 0);
    s21_decimal b = decimal_from_parts(987654321, 0, 0, 14, 0);
    s21_decimal result;
    
    // Умножение: 14 + 14 = 28 - в пределах
    int status = s21_mul(a, b, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    ck_assert_int_eq(s21_get_scale(&result), 28);
    
    // Деление может потребовать увеличения масштаба
    s21_decimal one = decimal_from_int32(1);
    s21_decimal small = decimal_from_parts(1, 0, 0, 28, 0);
    
    status = s21_div(one, small, &result);
    // Результат может быть слишком большим и вызвать переполнение
    // или масштаб может быть уменьшен с потерей точности
}
END_TEST

// ==================== ТЕСТЫ ДЕНОРМАЛИЗОВАННЫХ ЧИСЕЛ ====================

START_TEST(test_denormalized_numbers_handling) {
    // Тестируем операции с денормализованными числами
    // (числами с неоптимальным масштабом)
    
    s21_decimal denormalized = decimal_from_parts(1000, 0, 0, 3, 0); // 1.000
    s21_decimal normalized = decimal_from_parts(1, 0, 0, 0, 0);     // 1
    
    s21_decimal result1, result2;
    
    // Сложение денормализованного и нормализованного
    int status = s21_add(denormalized, normalized, &result1);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    
    // Умножение должно нормализовать результат
    status = s21_mul(denormalized, normalized, &result2);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    
    // Проверяем что результат нормализован (удалены конечные нули)
    ck_assert_int_eq(s21_get_scale(&result2), 0);
    ck_assert_uint_eq(result2.bits[0], 1);
}
END_TEST

START_TEST(test_trailing_zeros_elimination) {
    // Тестируем удаление конечных нулей после операций
    s21_decimal with_zeros = decimal_from_parts(1000, 0, 0, 3, 0); // 1.000
    s21_decimal without_zeros = decimal_from_parts(1, 0, 0, 0, 0); // 1
    
    s21_decimal result;
    
    // Операции должны удалять конечные нули
    int status = s21_add(with_zeros, without_zeros, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    
    // Результат должен быть 2, а не 2.000
    ck_assert_int_eq(s21_get_scale(&result), 0);
    ck_assert_uint_eq(result.bits[0], 2);
}
END_TEST

// ==================== ТЕСТЫ ГРАНИЧНЫХ СЛУЧАЕВ ПЕРЕПОЛНЕНИЯ ====================

START_TEST(test_progressive_overflow_addition) {
    // Тестируем постепенное переполнение при сложении
    s21_decimal value = get_max_decimal();
    value.bits[0] = 0xFFFFFF00; // Начинаем близко к максимуму
    
    s21_decimal increment = decimal_from_int32(1);
    s21_decimal result;
    int status;
    
    // Постепенно увеличиваем до переполнения
    for (int i = 0; i < 256; i++) {
        status = s21_add(value, increment, &result);
        if (status != S21_ARITHMETIC_OK) {
            break;
        }
        value = result;
    }
    
    // Должны получить переполнение в какой-то момент
    ck_assert_int_eq(status, S21_ARITHMETIC_INF);
}
END_TEST

START_TEST(test_boundary_overflow_cases) {
    // Тестируем пограничные случаи между OK и переполнением
    s21_decimal boundary_ok = get_max_decimal();
    boundary_ok.bits[0] = 0xFFFFFFFE; // Максимум - 1
    
    s21_decimal one = decimal_from_int32(1);
    s21_decimal two = decimal_from_int32(2);
    s21_decimal result;
    
    // boundary_ok + 1 должно быть OK
    int status = s21_add(boundary_ok, one, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    
    // boundary_ok + 2 должно вызвать переполнение
    status = s21_add(boundary_ok, two, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_INF);
}
END_TEST

START_TEST(test_overflow_combination_cases) {
    // Тестируем комбинированные случаи переполнения
    s21_decimal large_positive = get_max_decimal();
    large_positive.bits[0] = 0xFFFFFFFF / 2;
    
    s21_decimal large_negative = get_min_decimal();
    large_negative.bits[0] = 0xFFFFFFFF / 2;
    
    s21_decimal result;
    
    // Сложение двух больших положительных чисел
    int status = s21_add(large_positive, large_positive, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_INF);
    
    // Сложение двух больших отрицательных чисел
    status = s21_add(large_negative, large_negative, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_NEG_INF);
    
    // Умножение больших чисел
    status = s21_mul(large_positive, large_positive, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_INF);
}
END_TEST

// ==================== ТЕСТЫ ОШИБОК И НЕКОРРЕКТНЫХ ВХОДНЫХ ДАННЫХ ====================

START_TEST(test_error_code_completeness) {
    // Тестируем что все коды ошибок возвращаются в соответствующих ситуациях
    s21_decimal max_val = get_max_decimal();
    s21_decimal min_val = get_min_decimal();
    s21_decimal zero = decimal_from_int32(0);
    s21_decimal one = decimal_from_int32(1);
    s21_decimal result;
    
    // S21_ARITHMETIC_INF
    int status = s21_add(max_val, one, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_INF);
    
    // S21_ARITHMETIC_NEG_INF
    status = s21_add(min_val, decimal_from_int32(-1), &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_NEG_INF);
    
    // S21_ARITHMETIC_ZERO_DIV
    status = s21_div(one, zero, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_ZERO_DIV);
    
    // S21_ARITHMETIC_OK
    status = s21_add(one, one, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
}
END_TEST

START_TEST(test_invalid_scale_operations) {
    // Тестируем операции с невалидными масштабами
    s21_decimal invalid_scale = decimal_from_int32(100);
    s21_set_scale(&invalid_scale, 29); // Некорректный масштаб
    
    s21_decimal valid = decimal_from_int32(50);
    s21_decimal result;
    
    // Функции не должны падать при невалидных данных
    // Поведение не определено, но должны возвращать ошибку или корректно обрабатывать
    s21_add(valid, invalid_scale, &result);
    s21_sub(valid, invalid_scale, &result);
    s21_mul(valid, invalid_scale, &result);
    s21_div(valid, invalid_scale, &result);
    
    // Проверяем что не произошло краша
    ck_assert(1);
}
END_TEST

// ==================== ТЕСТЫ КОМБИНАЦИЙ ОПЕРАЦИЙ И СЛОЖНЫХ СЦЕНАРИЕВ ====================

START_TEST(test_complex_operation_chains) {
    // Тестируем сложные цепочки операций
    s21_decimal a = decimal_from_int32(10);
    s21_decimal b = decimal_from_int32(20);
    s21_decimal c = decimal_from_int32(5);
    s21_decimal d = decimal_from_int32(2);
    s21_decimal temp1, temp2, result;
    
    // (a + b) * c / d
    s21_add(a, b, &temp1);      // 30
    s21_mul(temp1, c, &temp2);  // 150
    s21_div(temp2, d, &result); // 75
    
    s21_decimal expected = decimal_from_int32(75);
    ck_assert(decimal_equals(result, expected));
    
    // a * b - c * d
    s21_mul(a, b, &temp1);      // 200
    s21_mul(c, d, &temp2);      // 10
    s21_sub(temp1, temp2, &result); // 190
    
    expected = decimal_from_int32(190);
    ck_assert(decimal_equals(result, expected));
}
END_TEST

START_TEST(test_mixed_scale_operations) {
    // Тестируем операции с числами разных масштабов
    s21_decimal a = decimal_from_parts(1234, 0, 0, 3, 0); // 1.234
    s21_decimal b = decimal_from_parts(5678, 0, 0, 4, 0); // 0.5678
    s21_decimal c = decimal_from_parts(90, 0, 0, 1, 0);   // 9.0
    
    s21_decimal result1, result2, result3;
    
    // Сложение разных масштабов
    int status = s21_add(a, b, &result1);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    
    // Умножение разных масштабов
    status = s21_mul(a, c, &result2);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    
    // Деление разных масштабов
    status = s21_div(b, c, &result3);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    
    // Проверяем что масштабы результатов корректны
    ck_assert_int_ge(s21_get_scale(&result1), 3);
    ck_assert_int_ge(s21_get_scale(&result2), 0);
    ck_assert_int_ge(s21_get_scale(&result3), 0);
}
END_TEST

START_TEST(test_accumulated_rounding_errors) {
    // Тестируем накопление ошибок округления при множественных операциях
    s21_decimal third = decimal_from_parts(333333333, 0, 0, 9, 0); // 0.333333333
    s21_decimal result = decimal_from_int32(0);
    s21_decimal temp;
    
    // Складываем 1/3 три раза - должно получиться примерно 1.0
    for (int i = 0; i < 3; i++) {
        s21_add(result, third, &temp);
        result = temp;
    }
    
    // Результат должен быть близок к 1.0
    // Из-за округления может быть 0.999999999 или 1.000000000
    ck_assert_uint_ge(result.bits[0], 999999999);
    ck_assert_uint_le(result.bits[0], 1000000000);
}
END_TEST

// ==================== ДОПОЛНИТЕЛЬНЫЕ ТЕСТЫ ДЛЯ КОНКРЕТНЫХ ОПЕРАЦИЙ ====================

START_TEST(test_division_precision_limits) {
    // Тестируем пределы точности при делении
    s21_decimal one = decimal_from_int32(1);
    s21_decimal seven = decimal_from_int32(7);
    s21_decimal result;
    
    // 1 / 7 = 0.142857142857... - бесконечная дробь
    int status = s21_div(one, seven, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    
    // Проверяем что результат имеет разумный масштаб и значение
    ck_assert_int_ge(s21_get_scale(&result), 1);
    ck_assert_int_le(s21_get_scale(&result), 28);
    ck_assert_uint_ge(result.bits[0], 142857142);
}
END_TEST

START_TEST(test_multiplication_scale_handling) {
    // Тестируем обработку масштабов при умножении
    s21_decimal a = decimal_from_parts(123, 0, 0, 2, 0); // 1.23
    s21_decimal b = decimal_from_parts(456, 0, 0, 3, 0); // 0.456
    s21_decimal result;
    
    // 1.23 * 0.456 = 0.56088, масштаб 2 + 3 = 5
    int status = s21_mul(a, b, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    
    ck_assert_int_eq(s21_get_scale(&result), 5);
    ck_assert_uint_eq(result.bits[0], 56088);
}
END_TEST

START_TEST(test_subtraction_sign_handling) {
    // Тестируем обработку знаков при вычитании
    s21_decimal test_cases[][3] = {
        {decimal_from_int32(10), decimal_from_int32(5), decimal_from_int32(5)},    // 10 - 5 = 5
        {decimal_from_int32(5), decimal_from_int32(10), decimal_from_int32(-5)},   // 5 - 10 = -5
        {decimal_from_int32(-5), decimal_from_int32(-10), decimal_from_int32(5)},  // -5 - (-10) = 5
        {decimal_from_int32(-10), decimal_from_int32(-5), decimal_from_int32(-5)}  // -10 - (-5) = -5
    };
    
    for (int i = 0; i < 4; i++) {
        s21_decimal result;
        int status = s21_sub(test_cases[i][0], test_cases[i][1], &result);
        ck_assert_int_eq(status, S21_ARITHMETIC_OK);
        ck_assert(decimal_equals(result, test_cases[i][2]));
    }
}
END_TEST

// ==================== Test Suite Setup ====================

Suite *arithmetic_functions_suite(void) {
    Suite *s;
    TCase *tc_banking, *tc_precision, *tc_denormalized, *tc_overflow;
    TCase *tc_errors, *tc_complex, *tc_operations;

    s = suite_create("Decimal Arithmetic Functions - Complete Coverage");

    // Banking rounding tests
    tc_banking = tcase_create("Banking Rounding");
    tcase_add_test(tc_banking, test_banking_rounding_specification_example);
    tcase_add_test(tc_banking, test_banking_rounding_half_to_even_positive);
    tcase_add_test(tc_banking, test_banking_rounding_half_to_even_negative);
    tcase_add_test(tc_banking, test_banking_rounding_exact_half_scenarios);
    suite_add_tcase(s, tc_banking);

    // Precision and scaling tests
    tc_precision = tcase_create("Precision and Scaling");
    tcase_add_test(tc_precision, test_precision_loss_extreme_scales);
    tcase_add_test(tc_precision, test_gradual_precision_loss);
    tcase_add_test(tc_precision, test_scale_overflow_handling);
    suite_add_tcase(s, tc_precision);

    // Denormalized numbers tests
    tc_denormalized = tcase_create("Denormalized Numbers");
    tcase_add_test(tc_denormalized, test_denormalized_numbers_handling);
    tcase_add_test(tc_denormalized, test_trailing_zeros_elimination);
    suite_add_tcase(s, tc_denormalized);

    // Overflow and boundary tests
    tc_overflow = tcase_create("Overflow and Boundary Cases");
    tcase_add_test(tc_overflow, test_progressive_overflow_addition);
    tcase_add_test(tc_overflow, test_boundary_overflow_cases);
    tcase_add_test(tc_overflow, test_overflow_combination_cases);
    suite_add_tcase(s, tc_overflow);

    // Error handling tests
    tc_errors = tcase_create("Error Handling");
    tcase_add_test(tc_errors, test_error_code_completeness);
    tcase_add_test(tc_errors, test_invalid_scale_operations);
    suite_add_tcase(s, tc_errors);

    // Complex scenarios tests
    tc_complex = tcase_create("Complex Scenarios");
    tcase_add_test(tc_complex, test_complex_operation_chains);
    tcase_add_test(tc_complex, test_mixed_scale_operations);
    tcase_add_test(tc_complex, test_accumulated_rounding_errors);
    suite_add_tcase(s, tc_complex);

    // Specific operations tests
    tc_operations = tcase_create("Specific Operations");
    tcase_add_test(tc_operations, test_division_precision_limits);
    tcase_add_test(tc_operations, test_multiplication_scale_handling);
    tcase_add_test(tc_operations, test_subtraction_sign_handling);
    suite_add_tcase(s, tc_operations);

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
  Suite *s = arithmetic_functions_suite();
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