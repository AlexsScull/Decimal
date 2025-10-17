#include "../helpers/helpers.h"
#include "../s21_decimal.h"

// ============ ОБЪЯВЛЕНИЕ ФУНКЦИЙ, ОНИ ТАМА ВНИЗУ ================
static int s21_is_zero(s21_decimal value);
static int s21_is_valid_decimal(s21_decimal dec);
int s21_comparison_bits(s21_decimal a, s21_decimal b, int sign);
// ================================================================

// ================================================================
// также используется в other
typedef enum {
  S21_START = 3
  /*для проверки, что функция действительно работает,
  а не просто выводит то, что вложили в result в начале*/
} s21_bool_2;
// ================================================================

// ================ COMPARISON FUNCTION ===========================

// S21_TRUE если a < b, иначе S21_FALSE
int s21_is_less(s21_decimal a, s21_decimal b) {
  if (s21_is_valid_decimal(a) == S21_ERR ||
      s21_is_valid_decimal(b) == S21_ERR) {
    return S21_ERR;
  }
  int result = S21_START;
  int flag = 0;  // флаг для выхода
  if (s21_is_equal(a, b)) {
    result = S21_FALSE;
    flag = 1;
  }
  if ((!flag) && (s21_normalize(&a) != S21_OK || s21_normalize(&b) != S21_OK)) {
    flag = 1;
    result = S21_ERR;
  }
  if (!flag) {
    int sign_a = s21_get_sign(&a);
    int sign_b = s21_get_sign(&b);
    if (sign_a == 1 && sign_b == 0) {
      result = S21_TRUE;
      flag = 1;
    }
    if (sign_a == 0 && sign_b == 1) {
      result = S21_FALSE;
      flag = 1;
    }
    if (!flag) {
      result = s21_comparison_bits(a, b, sign_a);
    }
  }
  return result;
}

// S21_TRUE если a <= b, иначе S21_FALSE
int s21_is_less_or_equal(s21_decimal a, s21_decimal b) {
  if (s21_is_valid_decimal(a) == S21_ERR ||
      s21_is_valid_decimal(b) == S21_ERR) {
    return S21_ERR;
  }
  int result = S21_START;
  if (s21_is_equal(a, b)) {
    result = S21_TRUE;
  } else {
    if (s21_is_less(a, b)) {
      result = S21_TRUE;
    } else {
      result = S21_FALSE;
    }
  }
  return result;
}

// S21_TRUE если a > b, иначе S21_FALSE
int s21_is_greater(s21_decimal a, s21_decimal b) {
  if (s21_is_valid_decimal(a) == S21_ERR ||
      s21_is_valid_decimal(b) == S21_ERR) {
    return S21_ERR;
  }
  int result = S21_START;
  if (s21_is_less_or_equal(a, b)) {
    result = S21_FALSE;
  } else
    result = S21_TRUE;
  return result;
}

// S21_TRUE если a >= b, иначе S21_FALSE
int s21_is_greater_or_equal(s21_decimal a, s21_decimal b) {
  if (s21_is_valid_decimal(a) == S21_ERR ||
      s21_is_valid_decimal(b) == S21_ERR) {
    return S21_ERR;
  }
  int result = S21_START;
  if (s21_is_equal(a, b)) {
    result = S21_TRUE;
  } else {
    if (s21_is_greater(a, b)) {
      result = S21_TRUE;
    } else {
      result = S21_FALSE;
    }
  }
  return result;
}

// S21_TRUE если a != b, иначе S21_FALSE
int s21_is_not_equal(s21_decimal a, s21_decimal b) {
  if (s21_is_valid_decimal(a) == S21_ERR ||
      s21_is_valid_decimal(b) == S21_ERR) {
    return S21_ERR;
  }
  int result = S21_START;
  if (s21_is_equal(a, b)) {
    result = S21_FALSE;
  } else {
    result = S21_TRUE;
  }
  return result;
}

// S21_TRUE если a = b, иначе S21_FALSE
int s21_is_equal(s21_decimal a, s21_decimal b) {
  if (s21_is_valid_decimal(a) == S21_ERR ||
      s21_is_valid_decimal(b) == S21_ERR) {
    return S21_ERR;
  }
  int result = S21_START;
  int flag = 0;
  int normal_a = s21_normalize(&a);
  int normal_b = s21_normalize(&b);
  if (normal_a != S21_OK || normal_b != S21_OK) {
    flag = 1;
    result = S21_ERR;
  }
  if ((!flag) && s21_is_zero(a) && s21_is_zero(b)) {
    result = S21_TRUE;
    flag = 1;
  }
  if (!flag) {
    int sign_a = s21_get_sign(&a);
    int sign_b = s21_get_sign(&b);
    if (sign_a != sign_b) {
      result = S21_FALSE;
    } else {
      int scales = s21_align_scales(&a, &b);
      if (!scales) {
        if (a.bits[0] == b.bits[0] && a.bits[1] == b.bits[1] &&
            a.bits[2] == b.bits[2] && a.bits[3] == b.bits[3]) {
          result = S21_TRUE;
        } else {
          result = S21_FALSE;
        }
      } else
        result = scales;
    }
  }
  return result;
}

// ==================== Дополнительные функции ====================

// сонина функция из старой версии
static int s21_is_zero(s21_decimal value) {
  return value.bits[0] == 0 && value.bits[1] == 0 && value.bits[2] == 0;
}

/* проверка что ничего нет лишнего. ошибки - s21_error_code
 также используется в other */
static int s21_is_valid_decimal(s21_decimal dec) {
  // Проверка битов 24-30
  if ((dec.bits[3] & 0x7F000000) != 0) {
    return S21_ERR;
  }
  // Проверка диапазона масштаба (0-28)
  int scale = (dec.bits[3] >> 16) & 0xFF;
  if (scale < 0 || scale > 28) {
    return S21_ERR;
  }
  // Проверка битов 0-15
  if ((dec.bits[3] & 0x0000FFFF) != 0) {
    return S21_ERR;
  }
  return S21_OK;
}

// сравнение по int
int s21_comparison_bits(s21_decimal a, s21_decimal b, int sign) {
  int result = S21_START;
  if (s21_align_scales(&a, &b) == 0) {
    int flag = 0;
    int i = 2;
    while (i >= 0 && flag == 0) {
      if (a.bits[i] < b.bits[i]) {
        result = S21_TRUE;
        flag = 1;
      }
      if (a.bits[i] > b.bits[i]) {
        result = S21_FALSE;
        flag = 1;
      }
      --i;
    }
    if (sign) {
      if (result == S21_TRUE)
        result = S21_FALSE;
      else
        result = S21_TRUE;
    }
  }
  return result;
}

// ========== мусор, но пока пусть будет ===========

// int s21_comparison_bits(s21_decimal a, s21_decimal b, int sign) {
//   int result = S21_START;

//   if (s21_align_scales(&a, &b) == 0) {
//     int flag = 0;

//     int bit_a = 0;
//     int bit_b = 0;

//     int i = 95;
//     while (i >= 0 && flag == 0) {
//       bit_a = s21_get_bit(a, i);
//       bit_b = s21_get_bit(b, i);
//         if (bit_a > bit_b) {
//           result = S21_TRUE;
//           flag = 1;
//         }
//       --i;
//     }
//     if (flag == 0) {
//       result = S21_FALSE;
//     }
//     if (sign == 1) {
//       if (result == S21_TRUE){
//         result = S21_FALSE;
//       }
//       if (result == S21_FALSE)
//         result = S21_TRUE;
//     }
//   }
//   return result;
// }