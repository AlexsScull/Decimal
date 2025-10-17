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

// ==================== БАЗОВЫЕ АРИФМЕТИЧЕСКИЕ ТЕСТЫ ====================

START_TEST(test_add_basic) {
    s21_decimal a = decimal_from_int32(5);
    s21_decimal b = decimal_from_int32(3);
    s21_decimal result;
    
    int status = s21_add(a, b, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    
    s21_decimal expected = decimal_from_int32(8);
    ck_assert(decimal_equals(result, expected));
}
END_TEST

START_TEST(test_sub_basic) {
    s21_decimal a = decimal_from_int32(10);
    s21_decimal b = decimal_from_int32(4);
    s21_decimal result;
    
    int status = s21_sub(a, b, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    
    s21_decimal expected = decimal_from_int32(6);
    ck_assert(decimal_equals(result, expected));
}
END_TEST

START_TEST(test_mul_basic) {
    s21_decimal a = decimal_from_int32(7);
    s21_decimal b = decimal_from_int32(6);
    s21_decimal result;
    
    int status = s21_mul(a, b, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    
    s21_decimal expected = decimal_from_int32(42);
    ck_assert(decimal_equals(result, expected));
}
END_TEST

START_TEST(test_div_basic) {
    s21_decimal a = decimal_from_int32(15);
    s21_decimal b = decimal_from_int32(3);
    s21_decimal result;
    
    int status = s21_div(a, b, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    
    s21_decimal expected = decimal_from_int32(5);
    ck_assert(decimal_equals(result, expected));
}
END_TEST

// ==================== ТЕСТЫ С ДРОБНЫМИ ЧИСЛАМИ ====================

START_TEST(test_add_decimal) {
    s21_decimal a = decimal_from_parts(123, 0, 0, 2, 0); // 1.23
    s21_decimal b = decimal_from_parts(456, 0, 0, 3, 0); // 0.456
    s21_decimal result;
    
    int status = s21_add(a, b, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    
    // 1.23 + 0.456 = 1.686
    s21_decimal expected = decimal_from_parts(1686, 0, 0, 3, 0);
    ck_assert(decimal_equals(result, expected));
}
END_TEST

START_TEST(test_sub_decimal) {
    s21_decimal a = decimal_from_parts(150, 0, 0, 1, 0); // 15.0
    s21_decimal b = decimal_from_parts(75, 0, 0, 1, 0);  // 7.5
    s21_decimal result;
    
    int status = s21_sub(a, b, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    
    // 15.0 - 7.5 = 7.5
    s21_decimal expected = decimal_from_parts(75, 0, 0, 1, 0);
    ck_assert(decimal_equals(result, expected));
}
END_TEST

START_TEST(test_mul_decimal) {
    s21_decimal a = decimal_from_parts(125, 0, 0, 2, 0); // 1.25
    s21_decimal b = decimal_from_parts(200, 0, 0, 1, 0); // 20.0
    s21_decimal result;
    
    int status = s21_mul(a, b, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    
    // 1.25 * 20.0 = 25.0
    s21_decimal expected = decimal_from_parts(250, 0, 0, 1, 0);
    ck_assert(decimal_equals(result, expected));
}
END_TEST

START_TEST(test_div_decimal) {
    s21_decimal a = decimal_from_parts(125, 0, 0, 2, 0); // 1.25
    s21_decimal b = decimal_from_parts(50, 0, 0, 1, 0);  // 5.0
    s21_decimal result;
    
    int status = s21_div(a, b, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    
    // 1.25 / 5.0 = 0.25
    s21_decimal expected = decimal_from_parts(25, 0, 0, 2, 0);
    ck_assert(decimal_equals(result, expected));
}
END_TEST

// ==================== ТЕСТЫ С ОТРИЦАТЕЛЬНЫМИ ЧИСЛАМИ ====================

START_TEST(test_add_negative) {
    s21_decimal a = decimal_from_int32(-8);
    s21_decimal b = decimal_from_int32(3);
    s21_decimal result;
    
    int status = s21_add(a, b, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    
    s21_decimal expected = decimal_from_int32(-5);
    ck_assert(decimal_equals(result, expected));
}
END_TEST

START_TEST(test_sub_negative) {
    s21_decimal a = decimal_from_int32(5);
    s21_decimal b = decimal_from_int32(-3);
    s21_decimal result;
    
    int status = s21_sub(a, b, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    
    s21_decimal expected = decimal_from_int32(8);
    ck_assert(decimal_equals(result, expected));
}
END_TEST

START_TEST(test_mul_negative) {
    s21_decimal a = decimal_from_int32(-4);
    s21_decimal b = decimal_from_int32(5);
    s21_decimal result;
    
    int status = s21_mul(a, b, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    
    s21_decimal expected = decimal_from_int32(-20);
    ck_assert(decimal_equals(result, expected));
}
END_TEST

START_TEST(test_div_negative) {
    s21_decimal a = decimal_from_int32(-15);
    s21_decimal b = decimal_from_int32(3);
    s21_decimal result;
    
    int status = s21_div(a, b, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    
    s21_decimal expected = decimal_from_int32(-5);
    ck_assert(decimal_equals(result, expected));
}
END_TEST

// ==================== ТЕСТЫ ГРАНИЧНЫХ ЗНАЧЕНИЙ ====================

START_TEST(test_add_near_max) {
    s21_decimal near_max = get_max_decimal();
    near_max.bits[0] = 0xFFFFFFFE; // MAX - 1
    
    s21_decimal one = decimal_from_int32(1);
    s21_decimal result;
    
    int status = s21_add(near_max, one, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    
    s21_decimal expected = get_max_decimal();
    ck_assert(decimal_equals(result, expected));
}
END_TEST

START_TEST(test_add_overflow) {
    s21_decimal max_val = get_max_decimal();
    s21_decimal one = decimal_from_int32(1);
    s21_decimal result;
    
    int status = s21_add(max_val, one, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_INF);
}
END_TEST

START_TEST(test_sub_underflow) {
    s21_decimal min_val = get_min_decimal();
    s21_decimal one = decimal_from_int32(-1);
    s21_decimal result;
    
    int status = s21_sub(min_val, one, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_NEG_INF);
}
END_TEST

START_TEST(test_div_by_zero) {
    s21_decimal a = decimal_from_int32(10);
    s21_decimal zero = decimal_from_int32(0);
    s21_decimal result;
    
    int status = s21_div(a, zero, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_ZERO_DIV);
}
END_TEST

// ==================== ТЕСТЫ БАНКОВСКОГО ОКРУГЛЕНИЯ ====================

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

START_TEST(test_banking_rounding_half_to_even) {
    // Тестируем банковское округление (к ближайшему четному)
    // 2.5 -> 2 (к четному), 3.5 -> 4 (к четному)
    
    s21_decimal two_point_five = decimal_from_parts(25, 0, 0, 1, 0);
    s21_decimal three_point_five = decimal_from_parts(35, 0, 0, 1, 0);
    
    // Для тестирования округления используем деление
    s21_decimal two = decimal_from_int32(2);
    s21_decimal one = decimal_from_int32(1);
    s21_decimal result1, result2;
    
    // 5.0 / 2.0 = 2.5 -> должно округлиться до 2
    s21_decimal five = decimal_from_int32(5);
    int status = s21_div(five, two, &result1);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    
    // 7.0 / 2.0 = 3.5 -> должно округлиться до 4
    s21_decimal seven = decimal_from_int32(7);
    status = s21_div(seven, two, &result2);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    
    // Проверяем округление
    s21_decimal expected1 = decimal_from_int32(2);
    s21_decimal expected2 = decimal_from_int32(4);
    ck_assert(decimal_equals(result1, expected1));
    ck_assert(decimal_equals(result2, expected2));
}
END_TEST

// ==================== ТЕСТЫ КОМБИНАЦИЙ ОПЕРАЦИЙ ====================

START_TEST(test_complex_operation_chain) {
    // Тестируем сложные цепочки операций
    s21_decimal a = decimal_from_int32(10);
    s21_decimal b = decimal_from_int32(20);
    s21_decimal c = decimal_from_int32(5);
    s21_decimal d = decimal_from_int32(2);
    s21_decimal temp1, temp2, result;
    
    // (a + b) * c / d = (10 + 20) * 5 / 2 = 30 * 5 / 2 = 150 / 2 = 75
    s21_add(a, b, &temp1);      // 30
    s21_mul(temp1, c, &temp2);  // 150
    s21_div(temp2, d, &result); // 75
    
    s21_decimal expected = decimal_from_int32(75);
    ck_assert(decimal_equals(result, expected));
}
END_TEST

START_TEST(test_mixed_precision_operations) {
    // Тестируем операции с числами разной точности
    s21_decimal a = decimal_from_parts(1234, 0, 0, 3, 0); // 1.234
    s21_decimal b = decimal_from_parts(5678, 0, 0, 4, 0); // 0.5678
    s21_decimal result;
    
    // 1.234 + 0.5678 = 1.8018
    int status = s21_add(a, b, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    
    s21_decimal expected = decimal_from_parts(18018, 0, 0, 4, 0);
    ck_assert(decimal_equals(result, expected));
}
END_TEST

// ==================== ДОПОЛНИТЕЛЬНЫЕ ТЕСТЫ ====================

START_TEST(test_zero_operations) {
    s21_decimal zero = decimal_from_int32(0);
    s21_decimal five = decimal_from_int32(5);
    s21_decimal result;
    
    // 0 + 5 = 5
    int status = s21_add(zero, five, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    ck_assert(decimal_equals(result, five));
    
    // 5 - 0 = 5
    status = s21_sub(five, zero, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    ck_assert(decimal_equals(result, five));
    
    // 0 * 5 = 0
    status = s21_mul(zero, five, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    ck_assert(decimal_equals(result, zero));
    
    // 0 / 5 = 0
    status = s21_div(zero, five, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    ck_assert(decimal_equals(result, zero));
}
END_TEST

START_TEST(test_large_number_operations) {
    s21_decimal large1 = decimal_from_int64(1000000000);
    s21_decimal large2 = decimal_from_int64(2000000000);
    s21_decimal result;
    
    // 1000000000 + 2000000000 = 3000000000
    int status = s21_add(large1, large2, &result);
    ck_assert_int_eq(status, S21_ARITHMETIC_OK);
    
    s21_decimal expected = decimal_from_int64(3000000000);
    ck_assert(decimal_equals(result, expected));
}
END_TEST

// ==================== Test Suite Setup ====================

Suite *arithmetic_functions_suite(void) {
    Suite *s;
    TCase *tc_basic, *tc_decimal, *tc_negative, *tc_edge;
    TCase *tc_banking, *tc_complex, *tc_additional;

    s = suite_create("Decimal Arithmetic Functions - Complete Coverage");

    // Basic arithmetic tests
    tc_basic = tcase_create("Basic Arithmetic");
    tcase_add_test(tc_basic, test_add_basic);
    tcase_add_test(tc_basic, test_sub_basic);
    tcase_add_test(tc_basic, test_mul_basic);
    tcase_add_test(tc_basic, test_div_basic);
    suite_add_tcase(s, tc_basic);

    // Decimal number tests
    tc_decimal = tcase_create("Decimal Numbers");
    tcase_add_test(tc_decimal, test_add_decimal);
    tcase_add_test(tc_decimal, test_sub_decimal);
    tcase_add_test(tc_decimal, test_mul_decimal);
    tcase_add_test(tc_decimal, test_div_decimal);
    suite_add_tcase(s, tc_decimal);

    // Negative number tests
    tc_negative = tcase_create("Negative Numbers");
    tcase_add_test(tc_negative, test_add_negative);
    tcase_add_test(tc_negative, test_sub_negative);
    tcase_add_test(tc_negative, test_mul_negative);
    tcase_add_test(tc_negative, test_div_negative);
    suite_add_tcase(s, tc_negative);

    // Edge case tests
    tc_edge = tcase_create("Edge Cases");
    tcase_add_test(tc_edge, test_add_near_max);
    tcase_add_test(tc_edge, test_add_overflow);
    tcase_add_test(tc_edge, test_sub_underflow);
    tcase_add_test(tc_edge, test_div_by_zero);
    suite_add_tcase(s, tc_edge);

    // Banking rounding tests
    tc_banking = tcase_create("Banking Rounding");
    tcase_add_test(tc_banking, test_banking_rounding_specification_example);
    tcase_add_test(tc_banking, test_banking_rounding_half_to_even);
    suite_add_tcase(s, tc_banking);

    // Complex operation tests
    tc_complex = tcase_create("Complex Operations");
    tcase_add_test(tc_complex, test_complex_operation_chain);
    tcase_add_test(tc_complex, test_mixed_precision_operations);
    suite_add_tcase(s, tc_complex);

    // Additional tests
    tc_additional = tcase_create("Additional Tests");
    tcase_add_test(tc_additional, test_zero_operations);
    tcase_add_test(tc_additional, test_large_number_operations);
    suite_add_tcase(s, tc_additional);

    return s;
}

int main(int argc, char **argv) {
    int failed = 0;
    Suite *s = arithmetic_functions_suite();
    SRunner *runner = srunner_create(s);

    if (argc > 1 && strcmp(argv[1], "+") == 0) {
        srunner_run_all(runner, CK_VERBOSE);
    } else {
        srunner_run_all(runner, CK_NORMAL);
    }

    failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}