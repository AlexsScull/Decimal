#include <check.h>
#include <stdlib.h>
#include <string.h>

#include "../decimal/s21_decimal.h"

////////////////////////////////////////////
//                                        //
//              s21_negate                //
//                                        //
////////////////////////////////////////////

START_TEST(test_negate) {
  s21_decimal a = {{123456789, 0, 0, 0x00050000}};  // +1234.56789
  s21_decimal result = {{0, 0, 0, 0}};
  ck_assert_int_eq(s21_negate(a, &result), 0);

  s21_decimal expected = {{123456789, 0, 0, 0x80050000}};  // -1234.56789
  ck_assert_int_eq(s21_is_equal(result, expected),
                   1);  // result == expected (1) 
}
END_TEST

START_TEST(test_negate_positive_to_negativ) {
  s21_decimal a = {{544959, 0, 0, 0x00010000}};  // 54495.9
  s21_decimal result = {{0, 0, 0, 0}};  // сюда будем копировать
  ck_assert_int_eq(s21_negate(a, &result), 0);  // проверяем что всё успешно

  s21_decimal expected = {{544959, 0, 0, 0x80010000}};  // -54495.9
  ck_assert_int_eq(s21_is_equal(result, expected),
                   1);  // result == expected (1)  
}
END_TEST

START_TEST(test_negate_negative_to_positive) {
  s21_decimal a = {{123456, 0, 0, 0x80030000}};  // -123.456
  s21_decimal result = {{0, 0, 0, 0}};
  ck_assert_int_eq(s21_negate(a, &result), 0);

  s21_decimal expected = {{123456, 0, 0, 0x00030000}};  // +123.456
  ck_assert_int_eq(s21_is_equal(result, expected),
                   1);  // result == expected (1)
}
END_TEST

START_TEST(test_negate_zero_positive) {
  s21_decimal a = {{0, 0, 0, 0x00020000}};  // +0.00
  s21_decimal result = {{0, 0, 0, 0}};
  ck_assert_int_eq(s21_negate(a, &result), 0);

  s21_decimal expected = {{0, 0, 0, 0x80020000}};  // -0.00
  ck_assert_int_eq(s21_is_equal(result, expected),
                   1);  // result == expected (1)
}
END_TEST

START_TEST(test_negate_zero_negative) {
  s21_decimal a = {{0, 0, 0, 0x80010000}};  // -0.0
  s21_decimal result = {{0, 0, 0, 0}};
  ck_assert_int_eq(s21_negate(a, &result), 0);

  s21_decimal expected = {{0, 0, 0, 0x00010000}};  // +0.0
  ck_assert_int_eq(s21_is_equal(result, expected),
                   1);  // result == expected (1)
}
END_TEST

START_TEST(test_negate_normalise) {  // проверка с нормализацией?
  s21_decimal a = {{123450, 0, 0, 0x00040000}};  // 12.3450
  s21_decimal result = {{0, 0, 0, 0}};
  ck_assert_int_eq(s21_negate(a, &result), 0);

  s21_decimal expected = {{12345, 0, 0, 0x80030000}};  // 12.345
  ck_assert_int_eq(s21_is_equal(result, expected),
                   1);  // result == expected (1)
}
END_TEST

////////////////////////////////////////////
//                                        //
//              s21_round                 //
//                                        //
////////////////////////////////////////////

START_TEST(test_round_positive_round_down) {
  s21_decimal a = {{123456789, 0, 0, 0x00050000}};  // 1234.56789
  s21_decimal result = {{0, 0, 0, 0}};
  ck_assert_int_eq(s21_round(a, &result), 0);

  s21_decimal expected = {{1234, 0, 0, 0x00000000}};  // 1234
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_round_positive_round_up) {
  s21_decimal a = {{123456789, 0, 0, 0x00040000}};  // 12345.6789
  s21_decimal result = {{0, 0, 0, 0}};
  ck_assert_int_eq(s21_round(a, &result), 0);

  s21_decimal expected = {{12346, 0, 0, 0x00000000}};  // 12346
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_round_negative_round_down) {
  s21_decimal a = {{123456789, 0, 0, 0x80050000}};  // -1234.56789
  s21_decimal result = {{0, 0, 0, 0}};
  ck_assert_int_eq(s21_round(a, &result), 0);

  s21_decimal expected = {{1234, 0, 0, 0x80000000}};  // -1234
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_round_negative_round_up) {
  s21_decimal a = {{123456789, 0, 0, 0x80040000}};  // -12345.6789
  s21_decimal result = {{0, 0, 0, 0}};
  ck_assert_int_eq(s21_round(a, &result), 0);

  s21_decimal expected = {{12346, 0, 0, 0x80000000}};  // -12346
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_round_already_integer) {
  s21_decimal a = {{1234, 0, 0, 0x00000000}};  // 1234
  s21_decimal result = {{0, 0, 0, 0}};
  ck_assert_int_eq(s21_round(a, &result), 0);

  s21_decimal expected = {{1234, 0, 0, 0x00000000}};  // 1234
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_round_zero) {
  s21_decimal a = {{0, 0, 0, 0x00030000}};  // 0.000
  s21_decimal result = {{0, 0, 0, 0}};
  ck_assert_int_eq(s21_round(a, &result), 0);

  s21_decimal expected = {{0, 0, 0, 0x00000000}};  // 0
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_round_exactly_half) {
  s21_decimal a = {{12345, 0, 0, 0x00030000}};  // 12.345
  s21_decimal result = {{0, 0, 0, 0}};
  ck_assert_int_eq(s21_round(a, &result), 0);

  s21_decimal expected = {{12, 0, 0, 0x00000000}};  // 12
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

////////////////////////////////////////////
//                                        //
//              s21_truncate              //
//                                        //
////////////////////////////////////////////

START_TEST(test_truncate) {
  s21_decimal a = {{123456789, 0, 0, 0x00050000}};  // 1234.56789
  s21_decimal result = {{0, 0, 0, 0}};
  ck_assert_int_eq(s21_truncate(a, &result), 0);

  s21_decimal expected = {{1234, 0, 0, 0x00000000}};  // 1234
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_truncate_negative) {
  s21_decimal a = {{123456789, 0, 0, 0x80050000}};  // -1234.56789
  s21_decimal result = {{0, 0, 0, 0}};
  ck_assert_int_eq(s21_truncate(a, &result), 0);

  s21_decimal expected = {{1234, 0, 0, 0x80000000}};  // -1234
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_truncate_already_integer) {
  s21_decimal a = {{1234, 0, 0, 0x00000000}};  // 1234
  s21_decimal result = {{0, 0, 0, 0}};
  ck_assert_int_eq(s21_truncate(a, &result), 0);

  s21_decimal expected = {{1234, 0, 0, 0x00000000}};  // 1234
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_truncate_zero) {
  s21_decimal a = {{0, 0, 0, 0x00020000}};  // 0.00
  s21_decimal result = {{0, 0, 0, 0}};
  ck_assert_int_eq(s21_truncate(a, &result), 0);

  s21_decimal expected = {{0, 0, 0, 0x00000000}};  // 0
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_truncate_small_fraction) {
  s21_decimal a = {{1234567, 0, 0, 0x00060000}};  // 1.234567
  s21_decimal result = {{0, 0, 0, 0}};
  ck_assert_int_eq(s21_truncate(a, &result), 0);

  s21_decimal expected = {{1, 0, 0, 0x00000000}};  // 1
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

////////////////////////////////////////////
//                                        //
//              s21_floor                 //
//                                        //
////////////////////////////////////////////

////////////////////////////////////////////
//                                        //
//               Test Suite               //
//                                        //
////////////////////////////////////////////

Suite *sscanf_suite(void) {
  Suite *s = suite_create("s21_comparison");
  TCase *tc = tcase_create("Core");

  // s21_negate
  tcase_add_test(tc, test_negate);
  tcase_add_test(tc, test_negate_positive_to_negativ);
  tcase_add_test(tc, test_negate_negative_to_positive);
  tcase_add_test(tc, test_negate_zero_positive);
  tcase_add_test(tc, test_negate_zero_negative);
  tcase_add_test(tc, test_negate_normalise);

  // s21_round
  tcase_add_test(tc, test_round_positive_round_down);
  tcase_add_test(tc, test_round_positive_round_up);
  tcase_add_test(tc, test_round_negative_round_down);
  tcase_add_test(tc, test_round_negative_round_up);
  tcase_add_test(tc, test_round_already_integer);
  tcase_add_test(tc, test_round_zero);
  tcase_add_test(tc, test_round_exactly_half);

  // s21_truncate
  tcase_add_test(tc, test_truncate);
  tcase_add_test(tc, test_truncate_negative);
  tcase_add_test(tc, test_truncate_already_integer);
  tcase_add_test(tc, test_truncate_zero);
  tcase_add_test(tc, test_truncate_small_fraction);

  // s21_floor

  suite_add_tcase(s, tc);
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
  Suite *s = sscanf_suite();
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