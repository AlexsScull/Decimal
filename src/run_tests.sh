#!/bin/bash

# Массив с именами тестов для decimal
tests=("Arithmetic" "Comparison" "Conversion" "Other" "Helpers")

# Массив с командами для запуска тестов
commands=(
    "./obj_tests/test_arithmetic"
    "./obj_tests/test_comparison"
    "./obj_tests/test_conversion"
    "./obj_tests/test_other"
    "./obj_tests/test_helpers"
)

# Проверяем режим вывода и определяем тесты для запуска
verbose_mode=false
selected_tests=()

# Обрабатываем аргументы
for arg in "$@"; do
    if [ "$arg" == "+" ]; then
        verbose_mode=true
    else
        # Нормализуем аргумент (убираем ./ и путь)
        clean_arg="${arg#./}"
        clean_arg="${clean_arg##*/}"
        clean_arg="${clean_arg#test_}"  # Убираем префикс test_
        
        # Добавляем в список выбранных тестов
        selected_tests+=("$clean_arg")
    fi
done

# Если не выбраны конкретные тесты - используем все
if [ ${#selected_tests[@]} -eq 0 ]; then
    selected_tests=("${tests[@]}")
fi

# Проверяем существование тестовых программ
all_exist=true
for test_name in "${selected_tests[@]}"; do
    # Находим команду по имени теста
    found=false
    for i in "${!tests[@]}"; do
        if [[ "${tests[$i],,}" == "${test_name,,}" ]]; then
            if [ ! -f "${commands[$i]}" ]; then
                echo "Ошибка: Тестовая программа '${commands[$i]}' не найдена"
                all_exist=false
            fi
            found=true
            break
        fi
    done
    
    if [ "$found" == false ]; then
        echo "Ошибка: Неизвестный тест '$test_name'"
        echo "Доступные тесты: ${tests[*]}"
        all_exist=false
    fi
done

if ! $all_exist; then
    echo "Пожалуйста, сначала выполните 'make test'"
    exit 1
fi

# Переменная для хранения общего результата
all_success=true

echo "Запуск тестов для decimal..."
echo "========================================"

# Временный файл для хранения вывода тестов
temp_file=$(mktemp)

# Функция для цветного вывода
color_print() {
    local line="$1"
    local line_number="$2"
    
    # Первые две строки вывода - голубые
    if [ "$line_number" -le 2 ]; then
        echo -e "\e[36m$line\e[0m"
    # Строки с "Passed" - зеленые
    elif [[ "$line" =~ [Pp]assed ]]; then
        echo -e "\e[32m$line\e[0m"
    # Строки с "Failed" - красные
    elif [[ "$line" =~ [Ff]ailed ]]; then
        echo -e "\e[31m$line\e[0m"
    # Остальные строки - обычные
    else
        echo -e "\e[0m$line"
    fi
}

# Запускаем выбранные тесты
for test_name in "${selected_tests[@]}"; do
    # Находим команду по имени теста
    for i in "${!tests[@]}"; do
        if [[ "${tests[$i],,}" == "${test_name,,}" ]]; then
            echo "Тестируем ${tests[$i]} функции..."
            echo "----------------------------------------"
            
            # Формируем команду для запуска
            cmd="${commands[$i]}"
            if $verbose_mode; then
                cmd="$cmd +"
            fi
            
            # Запускаем тест
            $cmd > "$temp_file" 2>&1
            test_result=$?
            
            # Проверяем результат выполнения теста
            if [ $test_result -eq 0 ]; then
                echo -e "\e[32mТест ${tests[$i]} пройден успешно\e[0m"
            else
                echo -e "\e[31mТест ${tests[$i]} завершился с ошибкой\e[0m"
                all_success=false
            fi
            
            # Подробный вывод при необходимости
            if $verbose_mode; then
                echo "----------------------------------------"
                echo "Детали теста:"
                line_counter=0
                while IFS= read -r line; do
                    ((line_counter++))
                    color_print "$line" "$line_counter"
                done < "$temp_file"
            fi
            
            echo "----------------------------------------"
            echo ""
            break
        fi
    done
done

# Удаляем временный файл
rm -f "$temp_file"

# Выводим общий результат
echo "========================================"
if $all_success; then
    echo -e "\e[42m\e[30m ВСЕ ТЕСТЫ УСПЕШНО ПРОЙДЕНЫ \e[0m"
    exit 0
else
    echo -e "\e[41m\e[30m НЕКОТОРЫЕ ТЕСТЫ НЕ ПРОЙДЕНЫ \e[0m"
    exit 1
fi