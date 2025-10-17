#include <check.h>
#include <stdlib.h>
#include <string.h>

#include "../decimal/s21_decimal.h"

////////////////////////////////////////////
//                                        //
//              s21_is_less               //
//                                        //
////////////////////////////////////////////

// a < b  S21_TRUE (1)
// иначе  S21_FALSE (0)

START_TEST(test_is_less_less_positive_numbers) {  // (+) знак
  s21_decimal a = {{12355, 0, 0, 0x00040000}};    // 1.2355
  s21_decimal b = {{19355, 0, 0, 0x00040000}};    // 1.9355
  ck_assert_int_eq(s21_is_less(a, b), 1);         // a < b (1)
}
END_TEST

START_TEST(test_is_less_not_less_positive_numbers) {  // (+) знак
  s21_decimal a = {{3423400, 0, 0, 0x00020000}};      // 34234.00
  s21_decimal b = {{12355, 0, 0, 0x00040000}};        // 1.2355
  ck_assert_int_eq(s21_is_less(a, b), 0);             // a > b (0)
}
END_TEST

START_TEST(test_is_less_less_negative_numbers) {  // (-) знак
  s21_decimal a = {{183456, 0, 0, 0x80030000}};   // -183.456
  s21_decimal b = {{123456, 0, 0, 0x80030000}};   // -123.456
  ck_assert_int_eq(s21_is_less(a, b), 1);         // a < b (1)
}
END_TEST

START_TEST(test_is_less_not_less_negative_numbers) {  // (-) знак
  s21_decimal a = {{183456, 0, 0, 0x80030000}};       // -183.456
  s21_decimal b = {{999, 0, 0, 0x80000000}};          // -999
  ck_assert_int_eq(s21_is_less(a, b), 0);             // a > b (0)
}
END_TEST

START_TEST(test_is_less_different_signs_less) {  // разные знаки
  s21_decimal a = {{123456, 0, 0, 0x80030000}};  // -123.456
  s21_decimal b = {{544959, 0, 0, 0x00010000}};  // 54495.9
  ck_assert_int_eq(s21_is_less(a, b), 1);        // a < b (1)
}
END_TEST

START_TEST(test_is_less_different_signs_not_less) {  // разные знаки
  s21_decimal a = {{3423400, 0, 0, 0x00020000}};     // 34234.00
  s21_decimal b = {{123456, 0, 0, 0x80030000}};      // -123.456
  ck_assert_int_eq(s21_is_less(a, b), 0);            // a > b (0)
}
END_TEST

START_TEST(test_is_less_zero) {  // нули
  s21_decimal a = {{0, 0, 0, 0}};
  s21_decimal b = {{0, 0, 0, 0}};
  ck_assert_int_eq(s21_is_less(a, b), 0);  // a == b (0)
}
END_TEST

START_TEST(test_is_less_invalid_decimals) {  // Невалидные decimal структуры
  s21_decimal a = {{123, 0, 0, 0x00FF0000}};  // невалидный масштаб
  s21_decimal b = {{456, 0, 0, 0x00010000}};

  ck_assert_int_eq(s21_is_less(a, b), 4);
  // Должен возвращать ошибку или корректно обрабатывать
}
END_TEST

////////////////////////////////////////////
//                                        //
//        s21_is_less_or_equal            //
//                                        //
////////////////////////////////////////////

// a <= b  S21_TRUE (1)
// иначе   S21_FALSE (0)

START_TEST(test_is_less_or_equal_less_positive_numbers) {  // (+) знак
  s21_decimal a = {{12355, 0, 0, 0x00040000}};             // 1.2355
  s21_decimal b = {{19355, 0, 0, 0x00040000}};             // 1.9355
  ck_assert_int_eq(s21_is_less_or_equal(a, b), 1);         // a < b (1)
}
END_TEST

START_TEST(test_is_less_or_equal_equal_positive_numbers) {  // (+) знак
  s21_decimal a = {{12355, 0, 0, 0x00040000}};              // 1.2355
  s21_decimal b = {{12355, 0, 0, 0x00040000}};              // 1.2355
  ck_assert_int_eq(s21_is_less_or_equal(a, b), 1);          // a == b (1)
}
END_TEST

START_TEST(test_is_less_or_equal_not_less_positive_numbers) {  // (+) знак
  s21_decimal a = {{3423400, 0, 0, 0x00020000}};               // 34234.00
  s21_decimal b = {{12355, 0, 0, 0x00040000}};                 // 1.2355
  ck_assert_int_eq(s21_is_less_or_equal(a, b), 0);             // a > b (0)
}
END_TEST

START_TEST(test_is_less_or_equal_less_negative_numbers) {  // (-) знак
  s21_decimal a = {{183456, 0, 0, 0x80030000}};            // -183.456
  s21_decimal b = {{123456, 0, 0, 0x80030000}};            // -123.456
  ck_assert_int_eq(s21_is_less_or_equal(a, b), 1);         // a < b (1)
}
END_TEST

START_TEST(test_is_less_or_equal_equal_negative_numbers) {  // (-) знак
  s21_decimal a = {{123456, 0, 0, 0x80030000}};             // -123.456
  s21_decimal b = {{123456, 0, 0, 0x80030000}};             // -123.456
  ck_assert_int_eq(s21_is_less_or_equal(a, b), 1);          // a == b (1)
}
END_TEST

START_TEST(test_is_less_or_equal_not_less_negative_numbers) {  // (-) знак
  s21_decimal a = {{183456, 0, 0, 0x80030000}};                // -183.456
  s21_decimal b = {{999, 0, 0, 0x80000000}};                   // -999
  ck_assert_int_eq(s21_is_less_or_equal(a, b), 0);             // a > b (0)
}
END_TEST

START_TEST(test_is_less_or_equal_different_signs_less) {  // (разные знаки)
  s21_decimal a = {{123456, 0, 0, 0x80030000}};           // -123.456
  s21_decimal b = {{544959, 0, 0, 0x00010000}};           // 54495.9
  ck_assert_int_eq(s21_is_less_or_equal(a, b), 1);        // a < b (1)
}
END_TEST

START_TEST(test_is_less_or_equal_different_signs_not_less) {  // (разные знаки)
  s21_decimal a = {{3423400, 0, 0, 0x00020000}};              // 34234.00
  s21_decimal b = {{123456, 0, 0, 0x80030000}};               // -123.456
  ck_assert_int_eq(s21_is_less_or_equal(a, b), 0);            // a > b (0)
}
END_TEST

START_TEST(test_is_less_or_equal_zero) {  // нули
  s21_decimal a = {{0, 0, 0, 0}};
  s21_decimal b = {{0, 0, 0, 0}};
  ck_assert_int_eq(s21_is_less_or_equal(a, b), 1);  // a == b (1)
}
END_TEST

START_TEST(test_is_less_or_equal_different_scales) {  // проверка нормализации
  s21_decimal a = {{12345, 0, 0, 0x00030000}};      // 12.345
  s21_decimal b = {{123450, 0, 0, 0x00040000}};     // 12.3450
  ck_assert_int_eq(s21_is_less_or_equal(a, b), 1);  // a == b (1)
}
END_TEST

////////////////////////////////////////////
//                                        //
//            s21_is_greater              //
//                                        //
////////////////////////////////////////////

// a > b   S21_TRUE (1)
// иначе   S21_FALSE (0)

START_TEST(test_is_greater_greater_positive_numbers) {  // (+) знак
  s21_decimal a = {{19355, 0, 0, 0x00040000}};          // 1.9355
  s21_decimal b = {{12355, 0, 0, 0x00040000}};          // 1.2355
  ck_assert_int_eq(s21_is_greater(a, b), 1);            // a > b (1)
}
END_TEST

START_TEST(test_is_greater_not_greater_positive_numbers) {  // (+) знак
  s21_decimal a = {{12355, 0, 0, 0x00040000}};              // 1.2355
  s21_decimal b = {{3423400, 0, 0, 0x00020000}};            // 34234.00
  ck_assert_int_eq(s21_is_greater(a, b), 0);                // a < b (0)
}
END_TEST

START_TEST(test_is_greater_greater_negative_numbers) {  // (-) знак
  s21_decimal a = {{123456, 0, 0, 0x80030000}};         // -123.456
  s21_decimal b = {{183456, 0, 0, 0x80030000}};         // -183.456
  ck_assert_int_eq(s21_is_greater(a, b), 1);            // a > b (1)
}
END_TEST

START_TEST(test_is_greater_not_greater_negative_numbers) {  // (-) знак
  s21_decimal a = {{999, 0, 0, 0x80000000}};                // -999
  s21_decimal b = {{183456, 0, 0, 0x80030000}};             // -183.456
  ck_assert_int_eq(s21_is_greater(a, b), 0);                // a < b (0)
}
END_TEST

START_TEST(test_is_greater_different_signs_greater) {  // (разные) знаки
  s21_decimal a = {{544959, 0, 0, 0x00010000}};        // 54495.9
  s21_decimal b = {{123456, 0, 0, 0x80030000}};        // -123.456
  ck_assert_int_eq(s21_is_greater(a, b), 1);           // a > b (1)
}
END_TEST

START_TEST(test_is_greater_different_signs_not_greater) {  // (разные) знаки
  s21_decimal a = {{123456, 0, 0, 0x80030000}};            // -123.456
  s21_decimal b = {{3423400, 0, 0, 0x00020000}};           // 34234.00
  ck_assert_int_eq(s21_is_greater(a, b), 0);               // a < b (0)
}
END_TEST

START_TEST(test_is_greater_zero) {  // нули
  s21_decimal a = {{0, 0, 0, 0}};
  s21_decimal b = {{0, 0, 0, 0}};
  ck_assert_int_eq(s21_is_greater(a, b), 0);  // a == b (0)
}
END_TEST

START_TEST(test_is_greater_different_scales) {  // проверка нормализации
  s21_decimal a = {{12345, 0, 0, 0x00030000}};   // 12.345
  s21_decimal b = {{123450, 0, 0, 0x00040000}};  // 12.3450
  ck_assert_int_eq(s21_is_greater(a, b), 0);     // a == b (0)
}
END_TEST

////////////////////////////////////////////
//                                        //
//        s21_is_greater_or_equal         //
//                                        //
////////////////////////////////////////////

// a >= b  S21_TRUE (1)
// иначе   S21_FALSE (0)

START_TEST(test_is_greater_or_equal_greater_positive_numbers) {  // (+) знак
  s21_decimal a = {{19355, 0, 0, 0x00040000}};                   // 1.9355
  s21_decimal b = {{12355, 0, 0, 0x00040000}};                   // 1.2355
  ck_assert_int_eq(s21_is_greater_or_equal(a, b), 1);            // a > b (1)
}
END_TEST

START_TEST(test_is_greater_or_equal_equal_positive_numbers) {  // (+) знак
  s21_decimal a = {{12355, 0, 0, 0x00040000}};                 // 1.2355
  s21_decimal b = {{12355, 0, 0, 0x00040000}};                 // 1.2355
  ck_assert_int_eq(s21_is_greater_or_equal(a, b), 1);          // a == b (1)
}
END_TEST

START_TEST(test_is_greater_or_equal_not_greater_positive_numbers) {  // (+) знак
  s21_decimal a = {{12355, 0, 0, 0x00040000}};                       // 1.2355
  s21_decimal b = {{3423400, 0, 0, 0x00020000}};                     // 34234.00
  ck_assert_int_eq(s21_is_greater_or_equal(a, b), 0);  // a < b (0)
}
END_TEST

START_TEST(test_is_greater_or_equal_greater_negative_numbers) {  // (-) знак
  s21_decimal a = {{123456, 0, 0, 0x80030000}};                  // -123.456
  s21_decimal b = {{183456, 0, 0, 0x80030000}};                  // -183.456
  ck_assert_int_eq(s21_is_greater_or_equal(a, b), 1);            // a > b (1)
}
END_TEST

START_TEST(test_is_greater_or_equal_equal_negative_numbers) {  // (-) знак
  s21_decimal a = {{123456, 0, 0, 0x80030000}};                // -123.456
  s21_decimal b = {{123456, 0, 0, 0x80030000}};                // -123.456
  ck_assert_int_eq(s21_is_greater_or_equal(a, b), 1);          // a == b (1)
}
END_TEST

START_TEST(test_is_greater_or_equal_not_greater_negative_numbers) {  // (-) знак
  s21_decimal a = {{999, 0, 0, 0x80000000}};                         // -999
  s21_decimal b = {{183456, 0, 0, 0x80030000}};                      // -183.456
  ck_assert_int_eq(s21_is_greater_or_equal(a, b), 0);  // a < b (0)
}
END_TEST

START_TEST(
    test_is_greater_or_equal_different_signs_greater) {  // (разные) знаки
  s21_decimal a = {{544959, 0, 0, 0x00010000}};          // 54495.9
  s21_decimal b = {{123456, 0, 0, 0x80030000}};          // -123.456
  ck_assert_int_eq(s21_is_greater_or_equal(a, b), 1);  // a >= b (1)
}
END_TEST

START_TEST(
    test_is_greater_or_equal_different_signs_not_greater) {  // (разные) знаки
  s21_decimal a = {{123456, 0, 0, 0x80030000}};              // -123.456
  s21_decimal b = {{3423400, 0, 0, 0x00020000}};             // 34234.00
  ck_assert_int_eq(s21_is_greater_or_equal(a, b), 0);  // a < b (0)
}
END_TEST

START_TEST(test_is_greater_or_equal_zero) {  // нули
  s21_decimal a = {{0, 0, 0, 0}};
  s21_decimal b = {{0, 0, 0, 0}};
  ck_assert_int_eq(s21_is_greater_or_equal(a, b), 1);  // a == b (1)
}
END_TEST

START_TEST(
    test_is_greater_or_equal_different_scales) {  // проверка нормализации
  s21_decimal a = {{12345, 0, 0, 0x00030000}};         // 12.345
  s21_decimal b = {{123450, 0, 0, 0x00040000}};        // 12.3450
  ck_assert_int_eq(s21_is_greater_or_equal(a, b), 1);  // a == b (1)
}
END_TEST

////////////////////////////////////////////
//                                        //
//              s21_is_equal              //
//                                        //
////////////////////////////////////////////

// a == b  S21_TRUE (1)
// иначе   S21_FALSE (0)

START_TEST(test_is_equal_equal_positive_numbers) {  // (+) знак
  s21_decimal a = {{12355, 0, 0, 0x00040000}};      // 1.2355
  s21_decimal b = {{12355, 0, 0, 0x00040000}};      // 1.2355
  ck_assert_int_eq(s21_is_equal(a, b), 1);          // a == b (1)
}
END_TEST

START_TEST(test_is_equal_equal_negative_numbers) {  // (-) знак
  s21_decimal a = {{123456, 0, 0, 0x80030000}};     // -123.456
  s21_decimal b = {{123456, 0, 0, 0x80030000}};     // -123.456
  ck_assert_int_eq(s21_is_equal(a, b), 1);          // a == b (1)
}
END_TEST

START_TEST(test_is_equal_not_positive_numbers) {  // (+) знак
  s21_decimal a = {{544959, 0, 0, 0x00010000}};   // 54495.9
  s21_decimal b = {{3423400, 0, 0, 0x00020000}};  // 34234.00
  ck_assert_int_eq(s21_is_equal(a, b), 0);        // a != b (0)
}
END_TEST

START_TEST(test_is_equal_not_positive_numbers_2) {  // (+) знак
  s21_decimal a = {{12345, 0, 0, 0x00020000}};      // 123.45
  s21_decimal b = {{12346, 0, 0, 0x00020000}};      // 123.46
  ck_assert_int_eq(s21_is_equal(a, b), 0);          // a != b (0)
}
END_TEST

START_TEST(test_is_equal_not_positive_numbers_3) {  // (+) знак
  s21_decimal a = {{12345, 0, 0, 0x00020000}};      // 123.45
  s21_decimal b = {{123451, 0, 0, 0x00030000}};     // 123.451
  ck_assert_int_eq(s21_is_equal(a, b), 0);          // a != b (0)
}
END_TEST

START_TEST(test_is_equal_not_negative_numbers) {  // (-) знак
  s21_decimal a = {{123456, 0, 0, 0x80030000}};   // -123.456
  s21_decimal b = {{999, 0, 0, 0x80000000}};      // -999
  ck_assert_int_eq(s21_is_equal(a, b), 0);        // a != b (0)
}
END_TEST

START_TEST(test_is_equal_not_negative_numbers_2) {  // (-) знак
  s21_decimal a = {{123456, 0, 0, 0x80030000}};     // -123.456
  s21_decimal b = {{123456, 0, 0, 0x80040000}};     // -12.3456
  ck_assert_int_eq(s21_is_equal(a, b),
                   0);  // a != b (0)
}
END_TEST

START_TEST(test_is_equal_zero) {  // нули
  s21_decimal a = {{0, 0, 0, 0}};
  s21_decimal b = {{0, 0, 0, 0}};
  ck_assert_int_eq(s21_is_equal(a, b), 1);  // a == b (1)
}
END_TEST

START_TEST(test_is_equal_number_with_zero) {    // число и нуль
  s21_decimal a = {{12345, 0, 0, 0x00020000}};  // 123.45
  s21_decimal b = {{0, 0, 0, 0}};
  ck_assert_int_eq(s21_is_equal(a, b), 0);  // a != b (0)
}
END_TEST

START_TEST(test_is_equal_positive_negative_zero) {  // нули с разными знаками
  s21_decimal a = {{0, 0, 0, 0}};                   // +0
  s21_decimal b = {{0, 0, 0, 0x80000000}};  // -0
  ck_assert_int_eq(s21_is_equal(a, b), 1);  // a == b (1)
}

START_TEST(test_is_equal_scale) {  // проверка нормализации
  s21_decimal a = {{12345, 0, 0, 0x00020000}};   // 123.45
  s21_decimal b = {{123450, 0, 0, 0x00030000}};  // 123.450
  ck_assert_int_eq(s21_is_equal(a, b), 1);       // a == b (1)
}
END_TEST

START_TEST(test_is_equal_large_int) {  // большое число
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0, 0}};
  s21_decimal b = {{0xFFFFFFFF, 0xFFFFFFFF, 0, 0}};
  ck_assert_int_eq(s21_is_equal(a, b), 1);  // a == b (1)
}
END_TEST

START_TEST(test_is_equal_large_float) {  // большое дробное число
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0, 0x000A0000}};
  s21_decimal b = {{0xFFFFFFFF, 0xFFFFFFFF, 0, 0x000A0000}};
  ck_assert_int_eq(s21_is_equal(a, b), 1);  // a == b (1)
}
END_TEST

START_TEST(test_is_equal_max) {  // максимальное значение
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal b = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  ck_assert_int_eq(s21_is_equal(a, b), 1);  // a == b (1)
}
END_TEST

START_TEST(test_is_equal_min) {  // минимальное значение
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80000000}};
  s21_decimal b = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80000000}};
  ck_assert_int_eq(s21_is_equal(a, b), 1);  // a == b (1)
}
END_TEST

////////////////////////////////////////////
//                                        //
//           s21_is_not_equal             //
//                                        //
////////////////////////////////////////////

// a != b  S21_TRUE (1)
// иначе   S21_FALSE (0)

START_TEST(test_is_not_equal_yes_positive_numbers) {  // (+) знак
  s21_decimal a = {{544959, 0, 0, 0x00010000}};       // 54495.9
  s21_decimal b = {{3783400, 0, 0, 0x00020000}};      // 37834.00
  ck_assert_int_eq(s21_is_not_equal(a, b), 1);        // a != b (1)
}
END_TEST

START_TEST(test_is_not_equal_yes_positive_numbers_2) {  // (+) знак
  s21_decimal a = {{3783400, 0, 0, 0x00020000}};        // 37834.00
  s21_decimal b = {{3783480, 0, 0, 0x00020000}};        // 37834.80
  ck_assert_int_eq(s21_is_not_equal(a, b), 1);          // a != b (1)
}
END_TEST

START_TEST(test_is_not_equal_yes_positive_numbers_3) {  // (+) знак
  s21_decimal a = {{12345, 0, 0, 0x00020000}};          // 123.45
  s21_decimal b = {{123451, 0, 0, 0x00030000}};         // 123.451
  ck_assert_int_eq(s21_is_not_equal(a, b), 1);          // a != b (0)
}
END_TEST

START_TEST(test_is_not_equal_yes_negative_numbers) {  // (-) знак
  s21_decimal a = {{999, 0, 0, 0x80000000}};          // -999
  s21_decimal b = {{123456, 0, 0, 0x80030000}};       // -123.456
  ck_assert_int_eq(s21_is_not_equal(a, b), 1);        // a != b (1)
}
END_TEST

START_TEST(test_is_not_equal_yes_negative_numbers_2) {  // (-) знак
  s21_decimal a = {{999, 0, 0, 0x80000000}};            // -999
  s21_decimal b = {{989, 0, 0, 0x80000000}};            // -989
  ck_assert_int_eq(s21_is_not_equal(a, b), 1);          // a != b (1)
}
END_TEST

START_TEST(test_is_not_equal_zero) {  // нули
  s21_decimal a = {{0, 0, 0, 0}};
  s21_decimal b = {{0, 0, 0, 0}};
  ck_assert_int_eq(s21_is_not_equal(a, b), 0);  // a == b (0)
}
END_TEST

START_TEST(test_is_not_equal_number_with_zero) {  // число и нуль
  s21_decimal a = {{0, 0, 0, 0}};
  s21_decimal b = {{12345, 0, 0, 0x00020000}};  // 123.45
  ck_assert_int_eq(s21_is_not_equal(a, b), 1);  // a != b (1)
}
END_TEST

START_TEST(
    test_is_not_equal_positive_negative_zero) {  // нули с разными знаками
  s21_decimal a = {{0, 0, 0, 0}};                // +0
  s21_decimal b = {{0, 0, 0, 0x80000000}};      // -0
  ck_assert_int_eq(s21_is_not_equal(a, b), 0);  // a == b (0)
}

START_TEST(test_is_not_equal_scale) {  // проверка нормализации
  s21_decimal a = {{67843, 0, 0, 0x00020000}};   // 123.45
  s21_decimal b = {{395020, 0, 0, 0x00030000}};  // 395.020
  ck_assert_int_eq(s21_is_not_equal(a, b), 1);   // a != b (1)
}
END_TEST

START_TEST(test_is_not_equal_scale_2) {  // проверка нормализации 2.0
  s21_decimal a = {{395020, 0, 0, 0x00030000}};  // 395.020
  s21_decimal b = {{395020, 0, 0, 0x00030000}};  // 395.020
  ck_assert_int_eq(s21_is_not_equal(a, b), 0);   // a == b (0)
}
END_TEST

START_TEST(test_is_not_equal_large_int) {  // c большим числом
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0, 0}};
  s21_decimal b = {{12345, 0, 0, 0x00020000}};  // 123.45
  ck_assert_int_eq(s21_is_not_equal(a, b), 1);  // a != b (1)
}
END_TEST

START_TEST(test_is_not_equal_large_float) {  // с большим дробным числом
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0, 0x000A0000}};
  s21_decimal b = {{12345, 0, 0, 0x00020000}};  // 123.45
  ck_assert_int_eq(s21_is_not_equal(a, b), 1);  // a!== b (1)
}
END_TEST

START_TEST(test_is_not_equal_max) {  // с максимальным значением
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal b = {{999, 0, 0, 0x80000000}};    // -999
  ck_assert_int_eq(s21_is_not_equal(a, b), 1);  // a == b (1)
}
END_TEST

START_TEST(test_is_not_equal_min) {  // с минимальным значением
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80000000}};
  s21_decimal b = {{999, 0, 0, 0x80000000}};    // -999
  ck_assert_int_eq(s21_is_not_equal(a, b), 1);  // a == b (1)
}
END_TEST

////////////////////////////////////////////
//                                        //
//               Test Suite               //
//                                        //
////////////////////////////////////////////

Suite *sscanf_suite(void) {
  Suite *s = suite_create("s21_comparison");
  TCase *tc = tcase_create("Core");

  // s21_is_less
  tcase_add_test(tc, test_is_less_less_positive_numbers);
  tcase_add_test(tc, test_is_less_not_less_positive_numbers);
  tcase_add_test(tc, test_is_less_less_negative_numbers);
  tcase_add_test(tc, test_is_less_not_less_negative_numbers);
  tcase_add_test(tc, test_is_less_different_signs_less);
  tcase_add_test(tc, test_is_less_different_signs_not_less);
  tcase_add_test(tc, test_is_less_zero);
  tcase_add_test(tc, test_is_less_invalid_decimals);

  // s21_is_less_or_equal
  tcase_add_test(tc, test_is_less_or_equal_less_positive_numbers);
  tcase_add_test(tc, test_is_less_or_equal_equal_positive_numbers);
  tcase_add_test(tc, test_is_less_or_equal_not_less_positive_numbers);
  tcase_add_test(tc, test_is_less_or_equal_less_negative_numbers);
  tcase_add_test(tc, test_is_less_or_equal_equal_negative_numbers);
  tcase_add_test(tc, test_is_less_or_equal_not_less_negative_numbers);
  tcase_add_test(tc, test_is_less_or_equal_different_signs_less);
  tcase_add_test(tc, test_is_less_or_equal_different_signs_not_less);
  tcase_add_test(tc, test_is_less_or_equal_zero);
  tcase_add_test(tc, test_is_less_or_equal_different_scales);

  // s21_is_greater
  tcase_add_test(tc, test_is_greater_greater_positive_numbers);
  tcase_add_test(tc, test_is_greater_not_greater_positive_numbers);
  tcase_add_test(tc, test_is_greater_greater_negative_numbers);
  tcase_add_test(tc, test_is_greater_not_greater_negative_numbers);
  tcase_add_test(tc, test_is_greater_different_signs_greater);
  tcase_add_test(tc, test_is_greater_different_signs_not_greater);
  tcase_add_test(tc, test_is_greater_zero);
  tcase_add_test(tc, test_is_greater_different_scales);

  // s21_is_greater_or_equal
  tcase_add_test(tc, test_is_greater_or_equal_greater_positive_numbers);
  tcase_add_test(tc, test_is_greater_or_equal_equal_positive_numbers);
  tcase_add_test(tc, test_is_greater_or_equal_not_greater_positive_numbers);
  tcase_add_test(tc, test_is_greater_or_equal_greater_negative_numbers);
  tcase_add_test(tc, test_is_greater_or_equal_equal_negative_numbers);
  tcase_add_test(tc, test_is_greater_or_equal_not_greater_negative_numbers);
  tcase_add_test(tc, test_is_greater_or_equal_different_signs_greater);
  tcase_add_test(tc, test_is_greater_or_equal_different_signs_not_greater);
  tcase_add_test(tc, test_is_greater_or_equal_zero);
  tcase_add_test(tc, test_is_greater_or_equal_different_scales);

  // s21_is_equal
  tcase_add_test(tc, test_is_equal_equal_positive_numbers);
  tcase_add_test(tc, test_is_equal_equal_negative_numbers);
  tcase_add_test(tc, test_is_equal_not_positive_numbers);
  tcase_add_test(tc, test_is_equal_not_positive_numbers_2);
  tcase_add_test(tc, test_is_equal_not_positive_numbers_3);
  tcase_add_test(tc, test_is_equal_not_negative_numbers);
  tcase_add_test(tc, test_is_equal_not_negative_numbers_2);
  tcase_add_test(tc, test_is_equal_zero);
  tcase_add_test(tc, test_is_equal_number_with_zero);
  tcase_add_test(tc, test_is_equal_positive_negative_zero);
  tcase_add_test(tc, test_is_equal_scale);
  tcase_add_test(tc, test_is_equal_large_int);
  tcase_add_test(tc, test_is_equal_large_float);
  tcase_add_test(tc, test_is_equal_max);
  tcase_add_test(tc, test_is_equal_min);

  // s21_is_not_equal
  tcase_add_test(tc, test_is_not_equal_yes_positive_numbers);
  tcase_add_test(tc, test_is_not_equal_yes_positive_numbers_2);
  tcase_add_test(tc, test_is_not_equal_yes_positive_numbers_3);
  tcase_add_test(tc, test_is_not_equal_yes_negative_numbers);
  tcase_add_test(tc, test_is_not_equal_yes_negative_numbers_2);
  tcase_add_test(tc, test_is_not_equal_zero);
  tcase_add_test(tc, test_is_not_equal_number_with_zero);
  tcase_add_test(tc, test_is_not_equal_positive_negative_zero);
  tcase_add_test(tc, test_is_not_equal_scale);
  tcase_add_test(tc, test_is_not_equal_scale_2);
  tcase_add_test(tc, test_is_not_equal_large_int);
  tcase_add_test(tc, test_is_not_equal_large_float);
  tcase_add_test(tc, test_is_not_equal_max);
  tcase_add_test(tc, test_is_not_equal_min);

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