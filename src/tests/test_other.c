#include <check.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "../decimal/s21_decimal.h"
#include "../decimal/helpers/helpers.h"

// ==================== Вспомогательные функции ====================

static s21_decimal decimal_from_int64(int64_t value) {
    s21_decimal result = {0};
    if (value < 0) {
        s21_set_sign(&result, 1);
        value = -value;
    }
    result.bits[0] = (uint32_t)(value & 0xFFFFFFFF);
    result.bits[1] = (uint32_t)((value >> 32) & 0xFFFFFFFF);
    return result;
}

static int64_t decimal_to_int64(s21_decimal value) {
    int64_t result = (int64_t)value.bits[0] | ((int64_t)value.bits[1] << 32);
    if (s21_get_sign(&value)) {
        result = -result;
    }
    return result;
}

// ==================== s21_negate Tests ====================

START_TEST(test_negate_positive_to_negative) {
    s21_decimal value = {{123456, 0, 0, 0}};
    s21_decimal result;
    
    int status = s21_negate(value, &result);
    
    ck_assert_int_eq(status, S21_OTHER_FUNC_OK);
    ck_assert_int_eq(s21_get_sign(&result), 1);
    ck_assert_uint_eq(result.bits[0], 123456);
}
END_TEST

START_TEST(test_negate_negative_to_positive) {
    s21_decimal value = {{789012, 0, 0, 0}};
    s21_set_sign(&value, 1);
    s21_decimal result;
    
    int status = s21_negate(value, &result);
    
    ck_assert_int_eq(status, S21_OTHER_FUNC_OK);
    ck_assert_int_eq(s21_get_sign(&result), 0);
    ck_assert_uint_eq(result.bits[0], 789012);
}
END_TEST

START_TEST(test_negate_positive_zero) {
    s21_decimal value = {{0, 0, 0, 0}};
    s21_decimal result;
    
    int status = s21_negate(value, &result);
    
    ck_assert_int_eq(status, S21_OTHER_FUNC_OK);
    ck_assert_int_eq(s21_get_sign(&result), 1);
    ck_assert_uint_eq(result.bits[0], 0);
}
END_TEST

START_TEST(test_negate_negative_zero) {
    s21_decimal value = {{0, 0, 0, 0}};
    s21_set_sign(&value, 1);
    s21_decimal result;
    
    int status = s21_negate(value, &result);
    
    ck_assert_int_eq(status, S21_OTHER_FUNC_OK);
    ck_assert_int_eq(s21_get_sign(&result), 0);
    ck_assert_uint_eq(result.bits[0], 0);
}
END_TEST

START_TEST(test_negate_preserves_scale_and_bits) {
    s21_decimal value = {{12345, 67890, 13579, 0}};
    s21_set_scale(&value, 5);
    s21_decimal result;
    
    int status = s21_negate(value, &result);
    
    ck_assert_int_eq(status, S21_OTHER_FUNC_OK);
    ck_assert_int_eq(s21_get_scale(&result), 5);
    ck_assert_uint_eq(result.bits[0], 12345);
    ck_assert_uint_eq(result.bits[1], 67890);
    ck_assert_uint_eq(result.bits[2], 13579);
    ck_assert_int_eq(s21_get_sign(&result), 1);
}
END_TEST

START_TEST(test_negate_max_value) {
    s21_decimal value = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
    s21_set_scale(&value, 10);
    s21_decimal result;
    
    int status = s21_negate(value, &result);
    
    ck_assert_int_eq(status, S21_OTHER_FUNC_OK);
    ck_assert_int_eq(s21_get_sign(&result), 1);
    ck_assert_int_eq(s21_get_scale(&result), 10);
    ck_assert_uint_eq(result.bits[0], 0xFFFFFFFF);
    ck_assert_uint_eq(result.bits[1], 0xFFFFFFFF);
    ck_assert_uint_eq(result.bits[2], 0xFFFFFFFF);
}
END_TEST

// ==================== s21_round Tests ====================

START_TEST(test_round_positive_integer_unchanged) {
    s21_decimal value = {{123456, 0, 0, 0}};
    s21_decimal result;
    
    int status = s21_round(value, &result);
    
    ck_assert_int_eq(status, S21_OTHER_FUNC_OK);
    ck_assert_uint_eq(result.bits[0], 123456);
    ck_assert_int_eq(s21_get_scale(&result), 0);
    ck_assert_int_eq(s21_get_sign(&result), 0);
}
END_TEST

START_TEST(test_round_positive_round_down) {
    s21_decimal value = {{123449, 0, 0, 0}};
    s21_set_scale(&value, 2); // 1234.49
    s21_decimal result;
    
    int status = s21_round(value, &result);
    
    ck_assert_int_eq(status, S21_OTHER_FUNC_OK);
    ck_assert_uint_eq(result.bits[0], 1234);
    ck_assert_int_eq(s21_get_scale(&result), 0);
}
END_TEST

START_TEST(test_round_positive_round_up) {
    s21_decimal value = {{123451, 0, 0, 0}};
    s21_set_scale(&value, 2); // 1234.51
    s21_decimal result;
    
    int status = s21_round(value, &result);
    
    ck_assert_int_eq(status, S21_OTHER_FUNC_OK);
    ck_assert_uint_eq(result.bits[0], 1235);
    ck_assert_int_eq(s21_get_scale(&result), 0);
}
END_TEST

START_TEST(test_round_negative_round_down) {
    s21_decimal value = {{123449, 0, 0, 0}};
    s21_set_scale(&value, 2); // 1234.49
    s21_set_sign(&value, 1);
    s21_decimal result;
    
    int status = s21_round(value, &result);
    
    ck_assert_int_eq(status, S21_OTHER_FUNC_OK);
    ck_assert_uint_eq(result.bits[0], 1234);
    ck_assert_int_eq(s21_get_sign(&result), 1);
    ck_assert_int_eq(s21_get_scale(&result), 0);
}
END_TEST

START_TEST(test_round_negative_round_up) {
    s21_decimal value = {{123451, 0, 0, 0}};
    s21_set_scale(&value, 2); // 1234.51
    s21_set_sign(&value, 1);
    s21_decimal result;
    
    int status = s21_round(value, &result);
    
    ck_assert_int_eq(status, S21_OTHER_FUNC_OK);
    ck_assert_uint_eq(result.bits[0], 1235);
    ck_assert_int_eq(s21_get_sign(&result), 1);
    ck_assert_int_eq(s21_get_scale(&result), 0);
}
END_TEST

START_TEST(test_round_banking_half_to_even_positive_even) {
    s21_decimal value = {{25, 0, 0, 0}};
    s21_set_scale(&value, 1); // 2.5
    s21_decimal result;
    
    int status = s21_round(value, &result);
    
    ck_assert_int_eq(status, S21_OTHER_FUNC_OK);
    ck_assert_uint_eq(result.bits[0], 2); // 2.5 -> 2 (to even)
    ck_assert_int_eq(s21_get_scale(&result), 0);
}
END_TEST

START_TEST(test_round_banking_half_to_even_positive_odd) {
    s21_decimal value = {{35, 0, 0, 0}};
    s21_set_scale(&value, 1); // 3.5
    s21_decimal result;
    
    int status = s21_round(value, &result);
    
    ck_assert_int_eq(status, S21_OTHER_FUNC_OK);
    ck_assert_uint_eq(result.bits[0], 4); // 3.5 -> 4 (to even)
    ck_assert_int_eq(s21_get_scale(&result), 0);
}
END_TEST

START_TEST(test_round_banking_half_to_even_negative_even) {
    s21_decimal value = {{25, 0, 0, 0}};
    s21_set_scale(&value, 1); // 2.5
    s21_set_sign(&value, 1);
    s21_decimal result;
    
    int status = s21_round(value, &result);
    
    ck_assert_int_eq(status, S21_OTHER_FUNC_OK);
    ck_assert_uint_eq(result.bits[0], 2); // -2.5 -> -2 (to even)
    ck_assert_int_eq(s21_get_sign(&result), 1);
    ck_assert_int_eq(s21_get_scale(&result), 0);
}
END_TEST

START_TEST(test_round_banking_half_to_even_negative_odd) {
    s21_decimal value = {{35, 0, 0, 0}};
    s21_set_scale(&value, 1); // 3.5
    s21_set_sign(&value, 1);
    s21_decimal result;
    
    int status = s21_round(value, &result);
    
    ck_assert_int_eq(status, S21_OTHER_FUNC_OK);
    ck_assert_uint_eq(result.bits[0], 4); // -3.5 -> -4 (to even)
    ck_assert_int_eq(s21_get_sign(&result), 1);
    ck_assert_int_eq(s21_get_scale(&result), 0);
}
END_TEST

START_TEST(test_round_removes_trailing_zeros) {
    s21_decimal value = {{1234500, 0, 0, 0}};
    s21_set_scale(&value, 4); // 123.4500
    s21_decimal result;
    
    int status = s21_round(value, &result);
    
    ck_assert_int_eq(status, S21_OTHER_FUNC_OK);
    ck_assert_uint_eq(result.bits[0], 123);
    ck_assert_int_eq(s21_get_scale(&result), 0);
}
END_TEST

START_TEST(test_round_zero) {
    s21_decimal value = {{0, 0, 0, 0}};
    s21_set_scale(&value, 5); // 0.00000
    s21_decimal result;
    
    int status = s21_round(value, &result);
    
    ck_assert_int_eq(status, S21_OTHER_FUNC_OK);
    ck_assert_uint_eq(result.bits[0], 0);
    ck_assert_int_eq(s21_get_scale(&result), 0);
    ck_assert_int_eq(s21_get_sign(&result), 0);
}
END_TEST

// ==================== s21_truncate Tests ====================

START_TEST(test_truncate_positive_fraction) {
    s21_decimal value = {{123456, 0, 0, 0}};
    s21_set_scale(&value, 3); // 123.456
    s21_decimal result;
    
    int status = s21_truncate(value, &result);
    
    ck_assert_int_eq(status, S21_OTHER_FUNC_OK);
    ck_assert_uint_eq(result.bits[0], 123);
    ck_assert_int_eq(s21_get_scale(&result), 0);
}
END_TEST

START_TEST(test_truncate_negative_fraction) {
    s21_decimal value = {{123456, 0, 0, 0}};
    s21_set_scale(&value, 3); // 123.456
    s21_set_sign(&value, 1);
    s21_decimal result;
    
    int status = s21_truncate(value, &result);
    
    ck_assert_int_eq(status, S21_OTHER_FUNC_OK);
    ck_assert_uint_eq(result.bits[0], 123);
    ck_assert_int_eq(s21_get_scale(&result), 0);
    ck_assert_int_eq(s21_get_sign(&result), 1);
}
END_TEST

START_TEST(test_truncate_positive_no_fraction) {
    s21_decimal value = {{123000, 0, 0, 0}};
    s21_set_scale(&value, 3); // 123.000
    s21_decimal result;
    
    int status = s21_truncate(value, &result);
    
    ck_assert_int_eq(status, S21_OTHER_FUNC_OK);
    ck_assert_uint_eq(result.bits[0], 123);
    ck_assert_int_eq(s21_get_scale(&result), 0);
}
END_TEST

START_TEST(test_truncate_removes_all_fractional_digits) {
    s21_decimal value = {{123456789, 0, 0, 0}};
    s21_set_scale(&value, 5); // 1234.56789
    s21_decimal result;
    
    int status = s21_truncate(value, &result);
    
    ck_assert_int_eq(status, S21_OTHER_FUNC_OK);
    ck_assert_uint_eq(result.bits[0], 1234);
    ck_assert_int_eq(s21_get_scale(&result), 0);
}
END_TEST

START_TEST(test_truncate_zero) {
    s21_decimal value = {{0, 0, 0, 0}};
    s21_set_scale(&value, 5); // 0.00000
    s21_decimal result;
    
    int status = s21_truncate(value, &result);
    
    ck_assert_int_eq(status, S21_OTHER_FUNC_OK);
    ck_assert_uint_eq(result.bits[0], 0);
    ck_assert_int_eq(s21_get_scale(&result), 0);
}
END_TEST

START_TEST(test_truncate_removes_trailing_zeros) {
    s21_decimal value = {{12345000, 0, 0, 0}};
    s21_set_scale(&value, 5); // 123.45000
    s21_decimal result;
    
    int status = s21_truncate(value, &result);
    
    ck_assert_int_eq(status, S21_OTHER_FUNC_OK);
    ck_assert_uint_eq(result.bits[0], 123);
    ck_assert_int_eq(s21_get_scale(&result), 0);
}
END_TEST

// ==================== s21_floor Tests ====================

START_TEST(test_floor_positive_fraction) {
    s21_decimal value = {{123456, 0, 0, 0}};
    s21_set_scale(&value, 3); // 123.456
    s21_decimal result;
    
    int status = s21_floor(value, &result);
    
    ck_assert_int_eq(status, S21_OTHER_FUNC_OK);
    ck_assert_uint_eq(result.bits[0], 123);
    ck_assert_int_eq(s21_get_scale(&result), 0);
}
END_TEST

START_TEST(test_floor_positive_no_fraction) {
    s21_decimal value = {{123000, 0, 0, 0}};
    s21_set_scale(&value, 3); // 123.000
    s21_decimal result;
    
    int status = s21_floor(value, &result);
    
    ck_assert_int_eq(status, S21_OTHER_FUNC_OK);
    ck_assert_uint_eq(result.bits[0], 123);
    ck_assert_int_eq(s21_get_scale(&result), 0);
}
END_TEST

START_TEST(test_floor_negative_fraction_toward_negative_infinity) {
    s21_decimal value = {{123456, 0, 0, 0}};
    s21_set_scale(&value, 3); // 123.456
    s21_set_sign(&value, 1);
    s21_decimal result;
    
    int status = s21_floor(value, &result);
    
    ck_assert_int_eq(status, S21_OTHER_FUNC_OK);
    ck_assert_uint_eq(result.bits[0], 124); // -123.456 -> -124
    ck_assert_int_eq(s21_get_sign(&result), 1);
    ck_assert_int_eq(s21_get_scale(&result), 0);
}
END_TEST

START_TEST(test_floor_negative_no_fraction) {
    s21_decimal value = {{123000, 0, 0, 0}};
    s21_set_scale(&value, 3); // 123.000
    s21_set_sign(&value, 1);
    s21_decimal result;
    
    int status = s21_floor(value, &result);
    
    ck_assert_int_eq(status, S21_OTHER_FUNC_OK);
    ck_assert_uint_eq(result.bits[0], 123);
    ck_assert_int_eq(s21_get_sign(&result), 1);
    ck_assert_int_eq(s21_get_scale(&result), 0);
}
END_TEST

START_TEST(test_floor_negative_small_fraction) {
    s21_decimal value = {{1, 0, 0, 0}};
    s21_set_scale(&value, 3); // 0.001
    s21_set_sign(&value, 1);
    s21_decimal result;
    
    int status = s21_floor(value, &result);
    
    ck_assert_int_eq(status, S21_OTHER_FUNC_OK);
    ck_assert_uint_eq(result.bits[0], 1); // -0.001 -> -1
    ck_assert_int_eq(s21_get_sign(&result), 1);
    ck_assert_int_eq(s21_get_scale(&result), 0);
}
END_TEST

START_TEST(test_floor_positive_small_fraction) {
    s21_decimal value = {{1, 0, 0, 0}};
    s21_set_scale(&value, 3); // 0.001
    s21_decimal result;
    
    int status = s21_floor(value, &result);
    
    ck_assert_int_eq(status, S21_OTHER_FUNC_OK);
    ck_assert_uint_eq(result.bits[0], 0); // 0.001 -> 0
    ck_assert_int_eq(s21_get_scale(&result), 0);
}
END_TEST

START_TEST(test_floor_zero) {
    s21_decimal value = {{0, 0, 0, 0}};
    s21_set_scale(&value, 2); // 0.00
    s21_decimal result;
    
    int status = s21_floor(value, &result);
    
    ck_assert_int_eq(status, S21_OTHER_FUNC_OK);
    ck_assert_uint_eq(result.bits[0], 0);
    ck_assert_int_eq(s21_get_scale(&result), 0);
}
END_TEST

// ==================== Edge Cases and Boundary Tests ====================

START_TEST(test_null_result_pointer_returns_error) {
    s21_decimal value = {{123, 0, 0, 0}};
    
    ck_assert_int_eq(s21_negate(value, NULL), S21_OTHER_FUNC_ERROR);
    ck_assert_int_eq(s21_round(value, NULL), S21_OTHER_FUNC_ERROR);
    ck_assert_int_eq(s21_truncate(value, NULL), S21_OTHER_FUNC_ERROR);
    ck_assert_int_eq(s21_floor(value, NULL), S21_OTHER_FUNC_ERROR);
}
END_TEST

START_TEST(test_scale_zero_after_round_truncate_floor) {
    s21_decimal value = {{123456, 0, 0, 0}};
    s21_set_scale(&value, 3);
    s21_decimal result;
    
    s21_round(value, &result);
    ck_assert_int_eq(s21_get_scale(&result), 0);
    
    s21_truncate(value, &result);
    ck_assert_int_eq(s21_get_scale(&result), 0);
    
    s21_floor(value, &result);
    ck_assert_int_eq(s21_get_scale(&result), 0);
}
END_TEST

START_TEST(test_scale_preserved_after_negate) {
    s21_decimal value = {{123456, 0, 0, 0}};
    s21_set_scale(&value, 5);
    s21_decimal result;
    
    s21_negate(value, &result);
    ck_assert_int_eq(s21_get_scale(&result), 5);
}
END_TEST

START_TEST(test_unused_bits_remain_zero) {
    s21_decimal value = {{123456, 0, 0, 0}};
    s21_set_scale(&value, 2);
    s21_decimal result;
    
    s21_negate(value, &result);
    ck_assert_int_ne(s21_validate_unused_bits(&result), 0);
    
    s21_round(value, &result);
    ck_assert_int_ne(s21_validate_unused_bits(&result), 0);
    
    s21_truncate(value, &result);
    ck_assert_int_ne(s21_validate_unused_bits(&result), 0);
    
    s21_floor(value, &result);
    ck_assert_int_ne(s21_validate_unused_bits(&result), 0);
}
END_TEST

START_TEST(test_large_number_operations) {
    s21_decimal large_value = {{0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0}};
    s21_set_scale(&large_value, 5);
    s21_decimal result;
    
    ck_assert_int_eq(s21_negate(large_value, &result), S21_OTHER_FUNC_OK);
    ck_assert_int_eq(s21_round(large_value, &result), S21_OTHER_FUNC_OK);
    ck_assert_int_eq(s21_truncate(large_value, &result), S21_OTHER_FUNC_OK);
    ck_assert_int_eq(s21_floor(large_value, &result), S21_OTHER_FUNC_OK);
}
END_TEST

START_TEST(test_negative_zero_handling) {
    s21_decimal neg_zero = {{0, 0, 0, 0}};
    s21_set_sign(&neg_zero, 1);
    s21_decimal result;
    
    ck_assert_int_eq(s21_negate(neg_zero, &result), S21_OTHER_FUNC_OK);
    ck_assert_int_eq(s21_get_sign(&result), 0);
    
    ck_assert_int_eq(s21_round(neg_zero, &result), S21_OTHER_FUNC_OK);
    ck_assert_int_eq(s21_get_sign(&result), 1); // Отрицательный ноль остается отрицательным
    
    ck_assert_int_eq(s21_truncate(neg_zero, &result), S21_OTHER_FUNC_OK);
    ck_assert_int_eq(s21_get_sign(&result), 1);
    
    ck_assert_int_eq(s21_floor(neg_zero, &result), S21_OTHER_FUNC_OK);
    ck_assert_int_eq(s21_get_sign(&result), 1);
}
END_TEST

// ==================== Test Suite Setup ====================

Suite *other_functions_suite(void) {
    Suite *s;
    TCase *tc_core, *tc_edge;

    s = suite_create("Other Decimal Functions");

    // Core functionality tests
    tc_core = tcase_create("Core Functions");
    
    // Negate tests
    tcase_add_test(tc_core, test_negate_positive_to_negative);
    tcase_add_test(tc_core, test_negate_negative_to_positive);
    tcase_add_test(tc_core, test_negate_positive_zero);
    tcase_add_test(tc_core, test_negate_negative_zero);
    tcase_add_test(tc_core, test_negate_preserves_scale_and_bits);
    tcase_add_test(tc_core, test_negate_max_value);
    
    // Round tests
    tcase_add_test(tc_core, test_round_positive_integer_unchanged);
    tcase_add_test(tc_core, test_round_positive_round_down);
    tcase_add_test(tc_core, test_round_positive_round_up);
    tcase_add_test(tc_core, test_round_negative_round_down);
    tcase_add_test(tc_core, test_round_negative_round_up);
    tcase_add_test(tc_core, test_round_banking_half_to_even_positive_even);
    tcase_add_test(tc_core, test_round_banking_half_to_even_positive_odd);
    tcase_add_test(tc_core, test_round_banking_half_to_even_negative_even);
    tcase_add_test(tc_core, test_round_banking_half_to_even_negative_odd);
    tcase_add_test(tc_core, test_round_removes_trailing_zeros);
    tcase_add_test(tc_core, test_round_zero);
    
    // Truncate tests
    tcase_add_test(tc_core, test_truncate_positive_fraction);
    tcase_add_test(tc_core, test_truncate_negative_fraction);
    tcase_add_test(tc_core, test_truncate_positive_no_fraction);
    tcase_add_test(tc_core, test_truncate_removes_all_fractional_digits);
    tcase_add_test(tc_core, test_truncate_zero);
    tcase_add_test(tc_core, test_truncate_removes_trailing_zeros);
    
    // Floor tests
    tcase_add_test(tc_core, test_floor_positive_fraction);
    tcase_add_test(tc_core, test_floor_positive_no_fraction);
    tcase_add_test(tc_core, test_floor_negative_fraction_toward_negative_infinity);
    tcase_add_test(tc_core, test_floor_negative_no_fraction);
    tcase_add_test(tc_core, test_floor_negative_small_fraction);
    tcase_add_test(tc_core, test_floor_positive_small_fraction);
    tcase_add_test(tc_core, test_floor_zero);

    suite_add_tcase(s, tc_core);

    // Edge cases tests
    tc_edge = tcase_create("Edge Cases");
    tcase_add_test(tc_edge, test_null_result_pointer_returns_error);
    tcase_add_test(tc_edge, test_scale_zero_after_round_truncate_floor);
    tcase_add_test(tc_edge, test_scale_preserved_after_negate);
    tcase_add_test(tc_edge, test_unused_bits_remain_zero);
    tcase_add_test(tc_edge, test_large_number_operations);
    tcase_add_test(tc_edge, test_negative_zero_handling);

    suite_add_tcase(s, tc_edge);

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
  Suite *s = other_functions_suite();
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