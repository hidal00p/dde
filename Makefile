EXAMPLES_DIR := examples
SRC_DIR := src
LIB_DIR := lib
TESTS_DIR := tests

all: tools examples tests

tools:
	@echo "===== Making DDE main tool ====="
	@make -C $(SRC_DIR) ddetools
	@echo

libs: 
	@echo "===== Making libs ====="
	@make -C $(LIB_DIR)/utils
	@echo

examples: libs
	@echo "===== Making demos ====="
	@make -C $(EXAMPLES_DIR)
	@echo

tests: libs
	@echo "===== Making tests ====="
	@make -C $(TESTS_DIR)
	@echo 

format:
	@make -C $(SRC_DIR) format && echo "Formated dde"
	@make -C $(EXAMPLES_DIR) format && echo "Formatted example"
	@make -C $(TESTS_DIR) format && echo "Formatted tests"
	@black scripts/*.py

clean:
	@make -C $(SRC_DIR) clean
	@make -C $(LIB_DIR)/utils clean
	@make -C $(EXAMPLES_DIR) clean
	@make -C $(TESTS_DIR) clean
	rm -rf *.out

.PHONY: format examples tools tests
