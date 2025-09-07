# Определение операционной системы
OS := $(shell uname)

# Компилятор и флаги
CC := gcc
CFLAGS :=  											# Временная заглушка
# CFLAGS := -std=c11 -Wall -Wextra -Werror 			# Временная заглушка
GCOV_FLAGS := -fprofile-arcs -ftest-coverage

# Флаги для линковки с библиотеками
LIB_FLAGS := -lcheck -lm -lgcov
ifeq ($(OS), Linux)
  LIB_FLAGS += -lsubunit -lpthread
endif

# Имя библиотеки
LIB := s21_decimal.a

# Директории
SRC_DIR := decimal
TEST_DIR := tests
OBJ_DIR := obj
TEST_OUT_DIR := obj_tests
COVERAGE_DIR := coverage_report

# Поиск всех исходных файлов библиотеки
SRC_FILES := $(wildcard $(SRC_DIR)/*.c) \
             $(wildcard $(SRC_DIR)/arithmetic/*.c) \
             $(wildcard $(SRC_DIR)/comparison/*.c) \
             $(wildcard $(SRC_DIR)/conversion/*.c) \
             $(wildcard $(SRC_DIR)/other/*.c) \
             $(wildcard $(SRC_DIR)/helpers/*.c)

# Object files
OBJ := $(SRC_FILES:%.c=$(OBJ_DIR)/%.o)

# Test files
TEST_FILES := $(wildcard $(TEST_DIR)/*.c)
TEST_TARGETS := $(TEST_FILES:$(TEST_DIR)/%.c=$(TEST_OUT_DIR)/%)

# Основные цели
.PHONY: all $(LIB) test docs gcov_report style_check style_fix cppcheck leaks valgrind clean rebuild

# Стандартная сборка
all: $(LIB)

# Сборка библиотеки
$(LIB): $(OBJ)
	ar rcs $@ $^
	ranlib $@

# Компиляция объектных файлов
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Правило для сборки тестов
$(TEST_OUT_DIR)/%: $(TEST_DIR)/%.c $(LIB)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< $(LIB) $(LIB_FLAGS) -o $@

# Сборка и запуск всех тестов
test: $(TEST_TARGETS)
	@for test in $^; do \
		echo "Running $${test}..."; \
		./$${test}; \
	done


# Генерация документации Doxygen
docs:
	cd .. && doxygen Doxyfile
	@echo "Документация сгенерирована в папке ../docs/"

# Генерация отчета о покрытии кода
gcov_report: CFLAGS += $(GCOV_FLAGS)
gcov_report: clean $(LIB)
	@mkdir -p $(COVERAGE_DIR)
	@mkdir -p $(TEST_OUT_DIR)
	# Компилируем все тесты с покрытием
	@for test_file in $(TEST_FILES); do \
		test_name=$$(basename $$test_file .c); \
		echo "Compiling $$test_name..."; \
		$(CC) $(CFLAGS) $$test_file $(LIB) $(LIB_FLAGS) -o $(TEST_OUT_DIR)/$$test_name; \
	done
	# Запускаем все тесты для генерации данных покрытия
	@for test in $(notdir $(TEST_FILES:.c=)); do \
		echo "Running $${test}..."; \
		./$(TEST_OUT_DIR)/$${test}; \
	done
	# Собираем данные покрытия из obj/ (библиотека)
	lcov -t "gcov_report" -o $(COVERAGE_DIR)/coverage.info -c -d $(OBJ_DIR)
	# Удаляем из отчета системные файлы (игнорируем ошибку unused)
	lcov --remove $(COVERAGE_DIR)/coverage.info "/usr/*" --ignore-errors unused -o $(COVERAGE_DIR)/coverage_filtered.info
	# Генерируем HTML-отчет
	genhtml -o $(COVERAGE_DIR) $(COVERAGE_DIR)/coverage_filtered.info
	@echo "Coverage report generated in $(COVERAGE_DIR)/index.html"

# Проверка стиля кода
style_check:
	find . -name "*.h" -exec clang-format -n {} +
	find . -name "*.c" -exec clang-format -n {} +

# Автоисправление стиля кода
style_fix:
	find . -name "*.h" -exec clang-format -i {} +
	find . -name "*.c" -exec clang-format -i {} +

# Статический анализ кода
cppcheck:
	cppcheck --enable=warning,performance,portability,style \
	         --suppress=missingIncludeSystem \
	         --inline-suppr \
	         --error-exitcode=1 \
	         $(SRC_DIR) 2>&1 | grep -E "(error|warning|note|$$)" || echo "No issues found"

# Проверка утечек (macOS)
leaks: $(TEST_TARGETS)
ifeq ($(OS), Darwin)
	@for test in $(TEST_TARGETS); do \
		echo "================================================"; \
		echo "Checking $${test} for leaks..."; \
		echo "================================================"; \
		leaks -atExit -- $$test 2>/dev/null | grep -E "(Process|leaks Report Version|total leaked bytes|LEAK)" | grep -v "0 leaks" || echo "No leaks detected"; \
		echo ""; \
	done
else
	@echo "Leaks check is only available on macOS"
	@echo "Use 'make valgrind' for Linux"
endif

# Проверка утечек (Linux)
valgrind: $(TEST_TARGETS)
ifeq ($(OS), Linux)
	@for test in $(TEST_TARGETS); do \
		echo "================================================"; \
		echo "Running valgrind on $${test}..."; \
		echo "================================================"; \
		valgrind --leak-check=full \
		         --show-leak-kinds=definite \
		         --error-exitcode=1 \
		         --quiet \
		         $$test || echo "Valgrind found errors!"; \
		echo ""; \
	done
else
	@echo "Valgrind is only available on Linux"
	@echo "Use 'make leaks' for macOS"
endif

# Очистка
clean:
	rm -rf $(OBJ_DIR) $(TEST_OUT_DIR) $(COVERAGE_DIR) $(LIB)

# Полная пересборка
rebuild: clean all

# Показ помощи
help:
	@echo "Доступные цели:"
	@echo "  all          - сборка библиотеки (по умолчанию)"
	@echo "  test         - запуск тестов"
	@echo "  docs     	  - генерация документации Doxygen"
	@echo "  gcov_report  - генерация отчета о покрытии кода"
	@echo "  style_check  - проверка стиля кода"
	@echo "  style_fix    - автоисправление стиля кода"
	@echo "  cppcheck     - статический анализ кода"
	@echo "  leaks        - проверка утечек памяти (macOS)"
	@echo "  valgrind     - проверка утечек памяти (Linux)"
	@echo "  clean        - очистка проекта"
	@echo "  rebuild      - полная пересборка"
	@echo "  help         - показ этой справки"