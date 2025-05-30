TARGETS_DIR := targets
SRC_DIR := src
LIB_DIR := lib
TESTS_DIR := tests

all: tools targets tests

tools:
	@echo "===== Making DDE main tool ====="
	@make -C $(SRC_DIR) ddetools
	@echo

targets:
	@echo "===== Making demo target ====="
	@make -C $(TARGETS_DIR)
	@echo

tests:
	@echo "===== Making tests ====="
	@make -C $(TESTS_DIR)
	@echo 

format:
	@make -C $(SRC_DIR) format && echo "Formated dde"
	@make -C $(TARGETS_DIR) format && echo "Formatted tests"
	@make -C $(TESTS_DIR) format && echo "Formatted targets"
	@black scripts/*.py

clean:
	@make -C $(SRC_DIR) clean
	@make -C $(TARGETS_DIR) clean
	@make -C $(TESTS_DIR) clean
	rm -rf *.out

.PHONY: format targets tools tests
