#!/bin/bash

if [ $# -eq 0 ]; then
    echo "Usage: $0 <test_file.c>" >&2
    exit 1
fi

if [ ! -f "$1" ]; then
    echo "Error: File '$1' not found" >&2
    exit 1
fi

# Имя выходного файла
OUTPUT_FILE="test_cases.txt"

# Очищаем файл (если существует) или создаём новый
> "$OUTPUT_FILE"

# Ищем все макросы TEST_* и START_TEST, извлекаем имена тестов и записываем в файл
grep -Eo '(START_TEST|TEST_\w+)\([^,)]+' "$1" |
sed -E 's/(START_TEST|TEST_\w+)\(//' |
while IFS= read -r test_name; do
    echo "    tcase_add_test(tc, $test_name);" >> "$OUTPUT_FILE"
done

echo "Результат записан в $OUTPUT_FILE"


# TEST_SPRINTF_PERSENT TEST_SPRINTF_N TEST_SP_NULL TEST_SPRINTF TEST_SPRINTF_2 TEST_SPRINTF_3
