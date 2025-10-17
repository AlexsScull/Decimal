#include <check.h>
#include <limits.h>
#include <string.h>
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


static int decimals_are_equal(s21_decimal a, s21_decimal b) {
    return a.bits[0] == b.bits[0] && a.bits[1] == b.bits[1] && 
           a.bits[2] == b.bits[2] && a.bits[3] == b.bits[3];
}

// ==================== Базовые тесты сравнения ====================

START_TEST(test_comparison_basic_positive_numbers) {
    s21_decimal small = decimal_from_int32(5);
    s21_decimal large = decimal_from_int32(10);
    
    ck_assert_int_eq(s21_is_less(small, large), 1);
    ck_assert_int_eq(s21_is_less(large, small), 0);
    ck_assert_int_eq(s21_is_greater(small, large), 0);
    ck_assert_int_eq(s21_is_greater(large, small), 1);
    ck_assert_int_eq(s21_is_equal(small, large), 0);
    ck_assert_int_eq(s21_is_not_equal(small, large), 1);
}
END_TEST

START_TEST(test_comparison_basic_negative_numbers) {
    s21_decimal small_neg = decimal_from_int32(-10);
    s21_decimal large_neg = decimal_from_int32(-5);
    
    ck_assert_int_eq(s21_is_less(small_neg, large_neg), 1);
    ck_assert_int_eq(s21_is_less(large_neg, small_neg), 0);
    ck_assert_int_eq(s21_is_greater(small_neg, large_neg), 0);
    ck_assert_int_eq(s21_is_greater(large_neg, small_neg), 1);
}
END_TEST

START_TEST(test_comparison_positive_vs_negative) {
    s21_decimal positive = decimal_from_int32(5);
    s21_decimal negative = decimal_from_int32(-5);
    
    ck_assert_int_eq(s21_is_less(negative, positive), 1);
    ck_assert_int_eq(s21_is_less(positive, negative), 0);
    ck_assert_int_eq(s21_is_greater(negative, positive), 0);
    ck_assert_int_eq(s21_is_greater(positive, negative), 1);
}
END_TEST

// ==================== s21_is_less Tests ====================

START_TEST(test_less_positive_integers) {
    s21_decimal a = decimal_from_int32(100);
    s21_decimal b = decimal_from_int32(200);
    
    ck_assert_int_eq(s21_is_less(a, b), 1);
    ck_assert_int_eq(s21_is_less(b, a), 0);
}
END_TEST

START_TEST(test_less_negative_integers) {
    s21_decimal a = decimal_from_int32(-200);
    s21_decimal b = decimal_from_int32(-100);
    
    ck_assert_int_eq(s21_is_less(a, b), 1);
    ck_assert_int_eq(s21_is_less(b, a), 0);
}
END_TEST

START_TEST(test_less_mixed_signs) {
    s21_decimal negative = decimal_from_int32(-50);
    s21_decimal positive = decimal_from_int32(50);
    
    ck_assert_int_eq(s21_is_less(negative, positive), 1);
    ck_assert_int_eq(s21_is_less(positive, negative), 0);
}
END_TEST

START_TEST(test_less_with_same_scale) {
    s21_decimal a = decimal_from_parts(150, 0, 0, 1, 0); // 15.0
    s21_decimal b = decimal_from_parts(200, 0, 0, 1, 0); // 20.0
    
    ck_assert_int_eq(s21_is_less(a, b), 1);
}
END_TEST

START_TEST(test_less_with_different_scales) {
    s21_decimal a = decimal_from_parts(1234, 0, 0, 3, 0); // 1.234
    s21_decimal b = decimal_from_parts(123, 0, 0, 2, 0);  // 1.23
    
    ck_assert_int_eq(s21_is_less(a, b), 0); // 1.234 > 1.23
    ck_assert_int_eq(s21_is_less(b, a), 1);
}
END_TEST

START_TEST(test_less_equal_values) {
    s21_decimal a = decimal_from_int32(42);
    s21_decimal b = decimal_from_int32(42);
    
    ck_assert_int_eq(s21_is_less(a, b), 0);
}
END_TEST

START_TEST(test_less_zero_cases) {
    s21_decimal zero = decimal_from_int32(0);
    s21_decimal neg_zero = decimal_from_int32(0);
    s21_set_sign(&neg_zero, 1);
    
    ck_assert_int_eq(s21_is_less(zero, neg_zero), 0);
    ck_assert_int_eq(s21_is_less(neg_zero, zero), 0);
}
END_TEST

START_TEST(test_less_large_numbers) {
    s21_decimal max = decimal_from_parts(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0, 0);
    s21_decimal near_max = decimal_from_parts(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFE, 0, 0);
    
    ck_assert_int_eq(s21_is_less(near_max, max), 1);
    ck_assert_int_eq(s21_is_less(max, near_max), 0);
}
END_TEST

START_TEST(test_less_after_normalization) {
    s21_decimal a = decimal_from_parts(100, 0, 0, 1, 0); // 10.0
    s21_decimal b = decimal_from_parts(1000, 0, 0, 2, 0); // 10.00
    
    ck_assert_int_eq(s21_is_less(a, b), 0); // Должны быть равны после нормализации
    ck_assert_int_eq(s21_is_less(b, a), 0);
}
END_TEST

// ==================== s21_is_less_or_equal Tests ====================

START_TEST(test_less_equal_positive) {
    s21_decimal a = decimal_from_int32(10);
    s21_decimal b = decimal_from_int32(20);
    s21_decimal c = decimal_from_int32(10);
    
    ck_assert_int_eq(s21_is_less_or_equal(a, b), 1);
    ck_assert_int_eq(s21_is_less_or_equal(b, a), 0);
    ck_assert_int_eq(s21_is_less_or_equal(a, c), 1);
}
END_TEST

START_TEST(test_less_equal_negative) {
    s21_decimal a = decimal_from_int32(-20);
    s21_decimal b = decimal_from_int32(-10);
    s21_decimal c = decimal_from_int32(-20);
    
    ck_assert_int_eq(s21_is_less_or_equal(a, b), 1);
    ck_assert_int_eq(s21_is_less_or_equal(b, a), 0);
    ck_assert_int_eq(s21_is_less_or_equal(a, c), 1);
}
END_TEST

START_TEST(test_less_equal_mixed_signs) {
    s21_decimal negative = decimal_from_int32(-10);
    s21_decimal positive = decimal_from_int32(10);
    
    ck_assert_int_eq(s21_is_less_or_equal(negative, positive), 1);
    ck_assert_int_eq(s21_is_less_or_equal(positive, negative), 0);
}
END_TEST

// ==================== s21_is_greater Tests ====================

START_TEST(test_greater_positive_integers) {
    s21_decimal a = decimal_from_int32(200);
    s21_decimal b = decimal_from_int32(100);
    
    ck_assert_int_eq(s21_is_greater(a, b), 1);
    ck_assert_int_eq(s21_is_greater(b, a), 0);
}
END_TEST

START_TEST(test_greater_negative_integers) {
    s21_decimal a = decimal_from_int32(-100);
    s21_decimal b = decimal_from_int32(-200);
    
    ck_assert_int_eq(s21_is_greater(a, b), 1);
    ck_assert_int_eq(s21_is_greater(b, a), 0);
}
END_TEST

START_TEST(test_greater_mixed_signs) {
    s21_decimal positive = decimal_from_int32(50);
    s21_decimal negative = decimal_from_int32(-50);
    
    ck_assert_int_eq(s21_is_greater(positive, negative), 1);
    ck_assert_int_eq(s21_is_greater(negative, positive), 0);
}
END_TEST

START_TEST(test_greater_with_scale) {
    s21_decimal a = decimal_from_parts(1235, 0, 0, 3, 0); // 1.235
    s21_decimal b = decimal_from_parts(123, 0, 0, 2, 0);  // 1.23
    
    ck_assert_int_eq(s21_is_greater(a, b), 1);
    ck_assert_int_eq(s21_is_greater(b, a), 0);
}
END_TEST

// ==================== s21_is_greater_or_equal Tests ====================

START_TEST(test_greater_equal_positive) {
    s21_decimal a = decimal_from_int32(20);
    s21_decimal b = decimal_from_int32(10);
    s21_decimal c = decimal_from_int32(20);
    
    ck_assert_int_eq(s21_is_greater_or_equal(a, b), 1);
    ck_assert_int_eq(s21_is_greater_or_equal(b, a), 0);
    ck_assert_int_eq(s21_is_greater_or_equal(a, c), 1);
}
END_TEST

START_TEST(test_greater_equal_negative) {
    s21_decimal a = decimal_from_int32(-10);
    s21_decimal b = decimal_from_int32(-20);
    s21_decimal c = decimal_from_int32(-10);
    
    ck_assert_int_eq(s21_is_greater_or_equal(a, b), 1);
    ck_assert_int_eq(s21_is_greater_or_equal(b, a), 0);
    ck_assert_int_eq(s21_is_greater_or_equal(a, c), 1);
}
END_TEST

// ==================== s21_is_equal Tests ====================

START_TEST(test_equal_positive_numbers) {
    s21_decimal a = decimal_from_int32(100);
    s21_decimal b = decimal_from_int32(100);
    s21_decimal c = decimal_from_int32(101);
    
    ck_assert_int_eq(s21_is_equal(a, b), 1);
    ck_assert_int_eq(s21_is_equal(a, c), 0);
}
END_TEST

START_TEST(test_equal_negative_numbers) {
    s21_decimal a = decimal_from_int32(-50);
    s21_decimal b = decimal_from_int32(-50);
    s21_decimal c = decimal_from_int32(-51);
    
    ck_assert_int_eq(s21_is_equal(a, b), 1);
    ck_assert_int_eq(s21_is_equal(a, c), 0);
}
END_TEST

START_TEST(test_equal_mixed_signs_always_false) {
    s21_decimal positive = decimal_from_int32(25);
    s21_decimal negative = decimal_from_int32(-25);
    
    ck_assert_int_eq(s21_is_equal(positive, negative), 0);
}
END_TEST

START_TEST(test_equal_with_different_scales) {
    s21_decimal a = decimal_from_parts(1234, 0, 0, 3, 0); // 1.234
    s21_decimal b = decimal_from_parts(12340, 0, 0, 4, 0); // 1.2340
    s21_decimal c = decimal_from_parts(123, 0, 0, 2, 0);   // 1.23
    
    ck_assert_int_eq(s21_is_equal(a, b), 1); // Должны быть равны после нормализации
    ck_assert_int_eq(s21_is_equal(a, c), 0);
}
END_TEST

START_TEST(test_equal_zeros_positive_negative) {
    s21_decimal zero = decimal_from_int32(0);
    s21_decimal neg_zero = decimal_from_int32(0);
    s21_set_sign(&neg_zero, 1);
    
    // Положительный и отрицательный ноль должны считаться равными
    ck_assert_int_eq(s21_is_equal(zero, neg_zero), 1);
}
END_TEST

START_TEST(test_equal_large_numbers) {
    s21_decimal max1 = decimal_from_parts(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0, 0);
    s21_decimal max2 = decimal_from_parts(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0, 0);
    s21_decimal almost_max = decimal_from_parts(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFE, 0, 0);
    
    ck_assert_int_eq(s21_is_equal(max1, max2), 1);
    ck_assert_int_eq(s21_is_equal(max1, almost_max), 0);
}
END_TEST

// ==================== s21_is_not_equal Tests ====================

START_TEST(test_not_equal_positive) {
    s21_decimal a = decimal_from_int32(10);
    s21_decimal b = decimal_from_int32(20);
    s21_decimal c = decimal_from_int32(10);
    
    ck_assert_int_eq(s21_is_not_equal(a, b), 1);
    ck_assert_int_eq(s21_is_not_equal(a, c), 0);
}
END_TEST

START_TEST(test_not_equal_negative) {
    s21_decimal a = decimal_from_int32(-10);
    s21_decimal b = decimal_from_int32(-20);
    s21_decimal c = decimal_from_int32(-10);
    
    ck_assert_int_eq(s21_is_not_equal(a, b), 1);
    ck_assert_int_eq(s21_is_not_equal(a, c), 0);
}
END_TEST

START_TEST(test_not_equal_mixed_signs) {
    s21_decimal positive = decimal_from_int32(5);
    s21_decimal negative = decimal_from_int32(-5);
    
    ck_assert_int_eq(s21_is_not_equal(positive, negative), 1);
}
END_TEST

// ==================== Критические Edge Cases ====================

START_TEST(test_comparison_max_decimal_values) {
    s21_decimal max_val = decimal_from_parts(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0, 0);
    s21_decimal almost_max = decimal_from_parts(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFE, 0, 0);
    
    ck_assert_int_eq(s21_is_less(almost_max, max_val), 1);
    ck_assert_int_eq(s21_is_greater(max_val, almost_max), 1);
    ck_assert_int_eq(s21_is_equal(max_val, almost_max), 0);
}
END_TEST

START_TEST(test_comparison_min_decimal_values) {
    s21_decimal min_val = decimal_from_parts(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0, 1);
    s21_decimal almost_min = decimal_from_parts(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFE, 0, 1);
    
    ck_assert_int_eq(s21_is_less(min_val, almost_min), 1); // -max < -almost_max
    ck_assert_int_eq(s21_is_greater(almost_min, min_val), 1);
}
END_TEST

START_TEST(test_comparison_very_small_numbers) {
    s21_decimal tiny1 = decimal_from_parts(1, 0, 0, 28, 0); // 1e-28
    s21_decimal tiny2 = decimal_from_parts(2, 0, 0, 28, 0); // 2e-28
    s21_decimal zero = decimal_from_int32(0);
    
    ck_assert_int_eq(s21_is_less(tiny1, tiny2), 1);
    ck_assert_int_eq(s21_is_less(zero, tiny1), 1);
    ck_assert_int_eq(s21_is_less(tiny1, zero), 0);
}
END_TEST

START_TEST(test_comparison_scale_28_numbers) {
    s21_decimal a = decimal_from_parts(123456789, 0, 0, 28, 0);
    s21_decimal b = decimal_from_parts(123456789, 0, 0, 28, 0);
    s21_decimal c = decimal_from_parts(123456788, 0, 0, 28, 0);
    
    ck_assert_int_eq(s21_is_equal(a, b), 1);
    ck_assert_int_eq(s21_is_equal(a, c), 0);
    ck_assert_int_eq(s21_is_less(c, a), 1);
}
END_TEST

START_TEST(test_comparison_negative_with_different_scales) {
    s21_decimal a = decimal_from_parts(1234, 0, 0, 3, 1); // -1.234
    s21_decimal b = decimal_from_parts(123, 0, 0, 2, 1);  // -1.23
    
    ck_assert_int_eq(s21_is_less(a, b), 1); // -1.234 < -1.23
    ck_assert_int_eq(s21_is_greater(b, a), 1);
}
END_TEST

START_TEST(test_comparison_negative_zero_behavior) {
    s21_decimal pos_zero = decimal_from_int32(0);
    s21_decimal neg_zero = decimal_from_int32(0);
    s21_set_sign(&neg_zero, 1);
    
    // Все сравнения должны считать нули равными
    ck_assert_int_eq(s21_is_equal(pos_zero, neg_zero), 1);
    ck_assert_int_eq(s21_is_less(pos_zero, neg_zero), 0);
    ck_assert_int_eq(s21_is_greater(pos_zero, neg_zero), 0);
    ck_assert_int_eq(s21_is_less_or_equal(pos_zero, neg_zero), 1);
    ck_assert_int_eq(s21_is_greater_or_equal(pos_zero, neg_zero), 1);
}
END_TEST

START_TEST(test_comparison_overflow_scenarios) {
    // Тестирование пограничных случаев, которые могут вызвать переполнение
    s21_decimal large1 = decimal_from_parts(0xFFFFFFFF, 0, 0, 0, 0);
    s21_decimal large2 = decimal_from_parts(0xFFFFFFFE, 0, 0, 0, 0);
    
    ck_assert_int_eq(s21_is_less(large2, large1), 1);
    ck_assert_int_eq(s21_is_greater(large1, large2), 1);
}
END_TEST

// ==================== Test Suite Setup ====================

Suite *comparison_functions_suite(void) {
    Suite *s;
    TCase *tc_basic, *tc_less, *tc_less_equal, *tc_greater, *tc_greater_equal;
    TCase *tc_equal, *tc_not_equal, *tc_edge;

    s = suite_create("Decimal Comparison Functions");

    // Basic comparison tests
    tc_basic = tcase_create("Basic Comparison");
    tcase_add_test(tc_basic, test_comparison_basic_positive_numbers);
    tcase_add_test(tc_basic, test_comparison_basic_negative_numbers);
    tcase_add_test(tc_basic, test_comparison_positive_vs_negative);
    suite_add_tcase(s, tc_basic);

    // Less tests
    tc_less = tcase_create("Less");
    tcase_add_test(tc_less, test_less_positive_integers);
    tcase_add_test(tc_less, test_less_negative_integers);
    tcase_add_test(tc_less, test_less_mixed_signs);
    tcase_add_test(tc_less, test_less_with_same_scale);
    tcase_add_test(tc_less, test_less_with_different_scales);
    tcase_add_test(tc_less, test_less_equal_values);
    tcase_add_test(tc_less, test_less_zero_cases);
    tcase_add_test(tc_less, test_less_large_numbers);
    tcase_add_test(tc_less, test_less_after_normalization);
    suite_add_tcase(s, tc_less);

    // Less or equal tests
    tc_less_equal = tcase_create("Less Or Equal");
    tcase_add_test(tc_less_equal, test_less_equal_positive);
    tcase_add_test(tc_less_equal, test_less_equal_negative);
    tcase_add_test(tc_less_equal, test_less_equal_mixed_signs);
    suite_add_tcase(s, tc_less_equal);

    // Greater tests
    tc_greater = tcase_create("Greater");
    tcase_add_test(tc_greater, test_greater_positive_integers);
    tcase_add_test(tc_greater, test_greater_negative_integers);
    tcase_add_test(tc_greater, test_greater_mixed_signs);
    tcase_add_test(tc_greater, test_greater_with_scale);
    suite_add_tcase(s, tc_greater);

    // Greater or equal tests
    tc_greater_equal = tcase_create("Greater Or Equal");
    tcase_add_test(tc_greater_equal, test_greater_equal_positive);
    tcase_add_test(tc_greater_equal, test_greater_equal_negative);
    suite_add_tcase(s, tc_greater_equal);

    // Equal tests
    tc_equal = tcase_create("Equal");
    tcase_add_test(tc_equal, test_equal_positive_numbers);
    tcase_add_test(tc_equal, test_equal_negative_numbers);
    tcase_add_test(tc_equal, test_equal_mixed_signs_always_false);
    tcase_add_test(tc_equal, test_equal_with_different_scales);
    tcase_add_test(tc_equal, test_equal_zeros_positive_negative);
    tcase_add_test(tc_equal, test_equal_large_numbers);
    suite_add_tcase(s, tc_equal);

    // Not equal tests
    tc_not_equal = tcase_create("Not Equal");
    tcase_add_test(tc_not_equal, test_not_equal_positive);
    tcase_add_test(tc_not_equal, test_not_equal_negative);
    tcase_add_test(tc_not_equal, test_not_equal_mixed_signs);
    suite_add_tcase(s, tc_not_equal);

    // Edge cases tests
    tc_edge = tcase_create("Edge Cases");
    tcase_add_test(tc_edge, test_comparison_max_decimal_values);
    tcase_add_test(tc_edge, test_comparison_min_decimal_values);
    tcase_add_test(tc_edge, test_comparison_very_small_numbers);
    tcase_add_test(tc_edge, test_comparison_scale_28_numbers);
    tcase_add_test(tc_edge, test_comparison_negative_with_different_scales);
    tcase_add_test(tc_edge, test_comparison_negative_zero_behavior);
    tcase_add_test(tc_edge, test_comparison_overflow_scenarios);
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
  Suite *s = comparison_functions_suite();
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