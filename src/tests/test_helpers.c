#include <check.h>
#include <stdlib.h>
#include <string.h>

#include "../decimal/helpers/helpers.h"

START_TEST(test_get_bit_normal) {
  s21_decimal dec = {{0xAAAAAAAA, 0x55555555, 0x33333333, 0}};
  ck_assert_int_eq(s21_get_bit(dec, 0), 0);
  ck_assert_int_eq(s21_get_bit(dec, 1), 1);
  ck_assert_int_eq(s21_get_bit(dec, 31), 1);
  ck_assert_int_eq(s21_get_bit(dec, 32), 1);
}
END_TEST

START_TEST(test_get_bit_out_of_range) {
  s21_decimal dec = {{0}};
  ck_assert_int_eq(s21_get_bit(dec, -1), 0);
  ck_assert_int_eq(s21_get_bit(dec, 128), 0);
  ck_assert_int_eq(s21_get_bit(dec, 999), 0);
}
END_TEST

START_TEST(test_set_bit_normal) {
  s21_decimal dec = {{0}};

  s21_set_bit(&dec, 0, 1);
  ck_assert_int_eq(s21_get_bit(dec, 0), 1);

  s21_set_bit(&dec, 31, 1);
  ck_assert_int_eq(s21_get_bit(dec, 31), 1);

  s21_set_bit(&dec, 63, 1);
  ck_assert_int_eq(s21_get_bit(dec, 63), 1);
}
END_TEST

START_TEST(test_set_bit_out_of_range) {
  s21_decimal dec = {{0}};

  s21_set_bit(&dec, -1, 1);
  s21_set_bit(&dec, 128, 1);
  s21_set_bit(&dec, 999, 1);

  ck_assert_int_eq(dec.bits[0], 0);
  ck_assert_int_eq(dec.bits[1], 0);
  ck_assert_int_eq(dec.bits[2], 0);
  ck_assert_int_eq(dec.bits[3], 0);
}
END_TEST

START_TEST(test_get_set_sign) {
  s21_decimal dec = {{0}};

  s21_set_sign(&dec, 0);
  ck_assert_int_eq(s21_get_sign(dec), 0);

  s21_set_sign(&dec, 1);
  ck_assert_int_eq(s21_get_sign(dec), 1);

  s21_set_sign(&dec, 5);
  ck_assert_int_eq(s21_get_sign(dec), 1);
}
END_TEST

START_TEST(test_sign_doesnt_affect_other_bits) {
  s21_decimal dec = {{0x12345678, 0x9ABCDEF0, 0x11111111, 0x000F0000}};
  unsigned int original_bits3 = (unsigned int)dec.bits[3];

  s21_set_sign(&dec, 1);
  ck_assert_uint_eq((unsigned int)dec.bits[0], 0x12345678);
  ck_assert_uint_eq((unsigned int)dec.bits[1], 0x9ABCDEF0);
  ck_assert_uint_eq((unsigned int)dec.bits[2], 0x11111111);
  ck_assert_uint_eq((unsigned int)dec.bits[3] & 0x7FFFFFFF,
                    original_bits3 & 0x7FFFFFFF);
}
END_TEST

START_TEST(test_get_set_scale_normal) {
  s21_decimal dec = {{0}};

  for (int scale = 0; scale <= 28; scale++) {
    s21_set_scale(&dec, scale);
    ck_assert_int_eq(s21_get_scale(dec), scale);
  }
}
END_TEST

START_TEST(test_scale_out_of_range) {
  s21_decimal dec = {{0}};

  s21_set_scale(&dec, -1);
  ck_assert_int_eq(s21_get_scale(dec), 255);

  s21_set_scale(&dec, 29);
  ck_assert_int_eq(s21_get_scale(dec), 29);

  s21_set_scale(&dec, 255);
  ck_assert_int_eq(s21_get_scale(dec), 255);
}
END_TEST

START_TEST(test_scale_doesnt_affect_sign) {
  s21_decimal dec = {{0}};
  s21_set_sign(&dec, 1);
  s21_set_scale(&dec, 15);

  ck_assert_int_eq(s21_get_sign(dec), 1);
  ck_assert_int_eq(s21_get_scale(dec), 15);
}
END_TEST

START_TEST(test_is_zero_true) {
  s21_decimal zero = {{0, 0, 0, 0}};
  s21_decimal with_scale = {{0, 0, 0, 0x000F0000}};  // Только масштаб
  s21_decimal with_sign = {{0, 0, 0, 0x80000000}};  // Только знак

  ck_assert_int_eq(s21_is_zero(zero), 1);
  ck_assert_int_eq(s21_is_zero(with_scale), 1);
  ck_assert_int_eq(s21_is_zero(with_sign), 1);
}
END_TEST

START_TEST(test_is_zero_false) {
  s21_decimal non_zero = {{1, 0, 0, 0}};
  s21_decimal non_zero2 = {{0, 1, 0, 0}};
  s21_decimal non_zero3 = {{0, 0, 1, 0}};

  ck_assert_int_eq(s21_is_zero(non_zero), 0);
  ck_assert_int_eq(s21_is_zero(non_zero2), 0);
  ck_assert_int_eq(s21_is_zero(non_zero3), 0);
}
END_TEST

START_TEST(test_zero_decimal) {
  s21_decimal dec = {{0x12345678, 0x9ABCDEF0, 0x11111111, 0x800F0000}};
  s21_zero_decimal(&dec);

  ck_assert_int_eq(dec.bits[0], 0);
  ck_assert_int_eq(dec.bits[1], 0);
  ck_assert_int_eq(dec.bits[2], 0);
  ck_assert_int_eq(dec.bits[3], 0);
}
END_TEST

START_TEST(test_decimal_init_zero) {
  s21_decimal zero = s21_decimal_init_zero();

  ck_assert_int_eq(zero.bits[0], 0);
  ck_assert_int_eq(zero.bits[1], 0);
  ck_assert_int_eq(zero.bits[2], 0);
  ck_assert_int_eq(zero.bits[3], 0);
}
END_TEST

START_TEST(test_copy_decimal) {
  s21_decimal src = {{0x12345678, 0x9ABCDEF0, 0x11111111, 0x800F0000}};
  s21_decimal dest;

  s21_copy_decimal(src, &dest);

  ck_assert_uint_eq((unsigned int)dest.bits[0], (unsigned int)src.bits[0]);
  ck_assert_uint_eq((unsigned int)dest.bits[1], (unsigned int)src.bits[1]);
  ck_assert_uint_eq((unsigned int)dest.bits[2], (unsigned int)src.bits[2]);
  ck_assert_uint_eq((unsigned int)dest.bits[3], (unsigned int)src.bits[3]);
}
END_TEST

START_TEST(test_copy_to_null) {
  s21_decimal src = {{0x12345678, 0, 0, 0}};
  s21_copy_decimal(src, NULL);
}
END_TEST

START_TEST(test_align_scales_equal) {
  s21_decimal a = {{100, 0, 0, 0x00050000}};  // scale 5
  s21_decimal b = {{200, 0, 0, 0x00050000}};  // scale 5

  s21_decimal a_orig = a;
  s21_decimal b_orig = b;

  s21_align_scales(&a, &b);

  ck_assert_int_eq(s21_get_scale(a), 5);
  ck_assert_int_eq(s21_get_scale(b), 5);
  ck_assert_int_eq(a.bits[0], a_orig.bits[0]);
  ck_assert_int_eq(b.bits[0], b_orig.bits[0]);
}
END_TEST

START_TEST(test_align_scales_different) {
  s21_decimal a = {{100, 0, 0, 0x00020000}};  // scale 2
  s21_decimal b = {{200, 0, 0, 0x00050000}};  // scale 5

  s21_align_scales(&a, &b);

  ck_assert_int_eq(s21_get_scale(a), 5);
  ck_assert_int_eq(s21_get_scale(b), 5);
  ck_assert_int_eq(a.bits[0], 100000);  // 100 * 10^3
}
END_TEST

START_TEST(test_align_scales_zero) {
  s21_decimal a = {{0, 0, 0, 0x00030000}};  // scale 3
  s21_decimal b = {{0, 0, 0, 0x00070000}};  // scale 7

  s21_align_scales(&a, &b);

  ck_assert_int_eq(s21_get_scale(a), 7);
  ck_assert_int_eq(s21_get_scale(b), 7);
  ck_assert_int_eq(a.bits[0], 0);  // Ноль остается нулем
}
END_TEST

START_TEST(test_multiply_by_power10_normal) {
  s21_decimal dec = {{5, 0, 0, 0}};

  s21_multiply_by_power10(&dec, 3);
  ck_assert_int_eq(dec.bits[0], 5000);

  s21_multiply_by_power10(&dec, 2);
  ck_assert_int_eq(dec.bits[0], 500000);
}
END_TEST

START_TEST(test_multiply_by_power10_overflow) {
  s21_decimal dec = {
      {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};  // Максимальное 96-битное число

  int result = s21_multiply_by_power10(&dec, 1);  // Умножаем на 10
  ck_assert_int_ne(result, 0);  // Должна быть ошибка переполнения
}
END_TEST

START_TEST(test_divide_by_power10_normal) {
  s21_decimal dec = {{5000, 0, 0, 0}};
  s21_divide_by_power10(&dec, 3);
  ck_assert_int_eq(dec.bits[0], 5);
}
END_TEST

START_TEST(test_divide_by_power10_remainder) {
  s21_decimal dec = {{1234, 0, 0, 0}};
  s21_divide_by_power10(&dec, 2);
  ck_assert_int_eq(dec.bits[0], 12);  // Должно округляться
}
END_TEST

START_TEST(test_normalize_trailing_zeros) {
  s21_decimal dec = {{123400, 0, 0, 0x00050000}};  // 123.40000

  s21_normalize(&dec);
  ck_assert_int_eq(dec.bits[0], 1234);
  ck_assert_int_eq(s21_get_scale(dec), 3);
}
END_TEST

START_TEST(test_bank_round_even) {
  s21_decimal dec = {{12345, 0, 0, 0x00040000}};  // 1.2345

  s21_bank_round(&dec, 3);  // Округлить до 3 знаков
  ck_assert_int_eq(dec.bits[0], 1234);  // 1.234
}
END_TEST

START_TEST(test_bank_round_odd) {
  s21_decimal dec = {{12355, 0, 0, 0x00040000}};  // 1.2355

  s21_bank_round(&dec, 3);
  ck_assert_int_eq(dec.bits[0], 1236);  // 1.236 (банковское округление)
}
END_TEST

Suite *error_suite(void) {
  Suite *s = suite_create("test_helpers");
  TCase *tc = tcase_create("Core");

  tcase_add_test(tc, test_get_bit_normal);
  tcase_add_test(tc, test_get_bit_out_of_range);
  tcase_add_test(tc, test_set_bit_normal);
  tcase_add_test(tc, test_set_bit_out_of_range);
  tcase_add_test(tc, test_get_set_sign);
  tcase_add_test(tc, test_sign_doesnt_affect_other_bits);
  tcase_add_test(tc, test_get_set_scale_normal);
  tcase_add_test(tc, test_scale_out_of_range);
  tcase_add_test(tc, test_scale_doesnt_affect_sign);
  tcase_add_test(tc, test_is_zero_true);
  tcase_add_test(tc, test_is_zero_false);
  tcase_add_test(tc, test_zero_decimal);
  tcase_add_test(tc, test_decimal_init_zero);
  tcase_add_test(tc, test_copy_decimal);
  tcase_add_test(tc, test_copy_to_null);
  tcase_add_test(tc, test_align_scales_equal);
  tcase_add_test(tc, test_align_scales_different);
  tcase_add_test(tc, test_align_scales_zero);
  tcase_add_test(tc, test_multiply_by_power10_normal);
  tcase_add_test(tc, test_multiply_by_power10_overflow);
  tcase_add_test(tc, test_divide_by_power10_normal);
  tcase_add_test(tc, test_divide_by_power10_remainder);
  tcase_add_test(tc, test_normalize_trailing_zeros);
  tcase_add_test(tc, test_bank_round_even);
  tcase_add_test(tc, test_bank_round_odd);

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
  Suite *s = error_suite();
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