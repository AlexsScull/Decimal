#include "../helpers/helpers.h"
#include "../s21_decimal.h"

/* НАДО: нормализация прости господи + функция s21_floor */

// ================ ОБЪЯВЛЕНИЕ ФУНКЦИЙ, ОНИ ТАМА ВНИЗУ ===========
void s21_copy_decimal(s21_decimal src, s21_decimal *dest);
static int s21_is_valid_decimal(s21_decimal dec);
void s21_first_digit_remainder(s21_decimal value, int *last_num, int *flag);
void s21_round_up(s21_decimal value, s21_decimal *result, int sign, int *res);
/* украденные сонины функции из другого файла */
static int s21_is_zero(s21_decimal value);
static uint32_t s21_get_last_digit(s21_decimal value);
static int s21_multiply_by_10(s21_decimal *value);
static uint32_t s21_divide_by_10(s21_decimal *value);
// ================================================================

// ================================================================
/**
 * @brief Коды ошибок для функций other
 */
typedef enum {
  S21_OTHER_OK = 0, /**< успешно */
  S21_OTHER_ERR = 1 /**< ошибка вычисления */
} s21_error_code_other;

// также используется в comparison
typedef enum {
  S21_START = 3
  /*для проверки, что функция действительно работает,
  а не просто выводит то, что вложили в result в начале*/
} s21_bool_2;
// ================================================================

// ======================= OTHER FUNCTION ========================

// меняем знак. 0 - ОК, 1 - ошибка
int s21_negate(s21_decimal value, s21_decimal *result) {
  if (result == NULL || s21_is_valid_decimal(value) == S21_ERR) {
    return S21_OTHER_ERR;
  }
  int res = S21_START;
  s21_zero_decimal(result);
  s21_copy_decimal(value, result);
  int sign = s21_get_sign(result);
  if (sign) {
    s21_set_sign(result, 0);
    res = S21_OK;
  } else {
    s21_set_sign(result, 1);
    res = S21_OK;
  }
  return res;
}

// Округляет decimal до ближайшего целого числа
int s21_round(s21_decimal value, s21_decimal *result) {
  if (result == NULL || s21_is_valid_decimal(value) == S21_ERR) {
    return S21_OTHER_ERR;
  }
  int res = S21_START;
  int flag = 0;
  if (s21_is_zero(value)) {
    s21_copy_decimal(value, result);
    res = S21_OTHER_OK;
    flag = 1;
  }
  if (!flag) {
    int scale = s21_get_scale(&value);
    if (scale == 0) {
      s21_copy_decimal(value, result);
      res = S21_OTHER_OK;
    } else {
      int sign = s21_get_sign(&value);
      int last_num = 0;
      s21_first_digit_remainder(value, &last_num, &flag);
      if (last_num > 5 && flag == S21_OTHER_OK) {
        s21_round_up(value, result, sign, &res);
      } else {
        if (s21_truncate(value, result) != S21_OTHER_OK) {
          res = S21_OTHER_ERR;
        }
        res = S21_OTHER_OK;
      }
    }
  }
  return res;
}

// возвращает целую часть числа, на дробную нам всё равно
int s21_truncate(s21_decimal value, s21_decimal *result) {
  if (result == NULL || s21_is_valid_decimal(value) == S21_ERR) {
    return S21_OTHER_ERR;
  }
  int res = S21_START;
  int flag = 0;
  if (s21_is_zero(value)) {
    s21_copy_decimal(value, result);
    res = S21_OTHER_OK;
    flag = 1;
  }
  if (!flag) {
    int scale = s21_get_scale(&value);
    if (scale == 0) {
      s21_copy_decimal(value, result);
      res = S21_OTHER_OK;
      flag = 1;
    } else {
      int sign = s21_get_sign(&value);
      s21_decimal temp;
      s21_copy_decimal(value, &temp);
      s21_set_sign(&temp, 0);
      if (s21_divide_by_power10(&temp, scale) != S21_OK) {
        res = S21_OTHER_ERR;
      } else {
        s21_copy_decimal(temp, result);
        result->bits[3] = 0;
        s21_set_sign(result, sign);
        res = S21_OTHER_OK;
      }
    }
  }
  return res;
}

int s21_floor(s21_decimal value, s21_decimal *result) {}

// ==================== Дополнительные функции ====================

// сонина функция из старой версии
void s21_copy_decimal(s21_decimal src, s21_decimal *dest) {
  if (dest != NULL) {
    for (int i = 0; i < 4; i++) {
      dest->bits[i] = src.bits[i];
    }
  }
}

/* проверка что ничего нет лишнего. ошибки - s21_error_code
 также используется в comparison */
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

// подфункция (round) для округления вверх
void s21_round_up(s21_decimal value, s21_decimal *result, int sign, int *res) {
  s21_decimal temp_2;
  s21_decimal temp_3;
  s21_copy_decimal(value, &temp_2);
  s21_set_sign(&temp_2, 0);
  if (s21_truncate(temp_2, &temp_3) != S21_OTHER_OK) {
    *res = S21_OTHER_ERR;
  } else {
    if (s21_add_one(&temp_3) != S21_OK) {
      *res = S21_OTHER_ERR;
    } else {
      s21_set_sign(&temp_3, sign);
      s21_copy_decimal(temp_3, result);
      *res = S21_OTHER_OK;
    }
  }
}

/* подфункция (round)
суть:
умножаем на 10
берем конечную цифру мантисы, то есть 1 цифру остатка */
void s21_first_digit_remainder(s21_decimal value, int *last_num, int *flag) {
  s21_decimal temp;
  s21_copy_decimal(value, &temp);
  s21_set_sign(&temp, 0);
  s21_decimal temp_zerro = {{0, 0, 0, 0}};

  *flag = S21_OTHER_OK;
  if (s21_multiply_by_10(&temp) != S21_OK) {
    *flag = S21_OTHER_ERR;
  }
  if (!*flag && s21_truncate(temp, &temp_zerro) != S21_OTHER_OK) {
    *flag = S21_OTHER_ERR;
  }
  if (*flag == S21_OTHER_OK) {
    *last_num = s21_get_last_digit(temp_zerro);  // получаем последнюю цифру
  }
}

// украденные сонины функции из другого файла
static int s21_is_zero(s21_decimal value) {
  return value.bits[0] == 0 && value.bits[1] == 0 && value.bits[2] == 0;
}

static int s21_multiply_by_10(s21_decimal *value) {
  uint64_t carry = 0;

  for (int i = 0; i < 3; i++) {
    uint64_t temp = (uint64_t)value->bits[i] * 10 + carry;
    value->bits[i] = (uint32_t)(temp & 0xFFFFFFFF);
    carry = temp >> 32;
  }

  return (carry == 0) ? S21_OK : S21_INF;
}

static uint32_t s21_get_last_digit(s21_decimal value) {
  s21_decimal copy = value;
  return s21_divide_by_10(&copy);
}

static uint32_t s21_divide_by_10(s21_decimal *value) {
  uint64_t remainder = 0;

  for (int i = 2; i >= 0; i--) {
    uint64_t current_value = ((uint64_t)remainder << 32) | value->bits[i];
    value->bits[i] = (uint32_t)(current_value / 10);
    remainder = current_value % 10;
  }

  return (uint32_t)remainder;
} 