TARGETS_DIR := targets
SRC_DIR := src
LIB_DIR := lib
TESTS_DIR := tests

all: tools targets libs tests

tools:
	$(MAKE) -C $(SRC_DIR) tools

targets:
	$(MAKE) -C $(TARGETS_DIR)

tests: libs
	$(MAKE) -C $(TESTS_DIR) unit

libs:
	$(MAKE) -C $(LIB_DIR)/cppunitlite

format:
	@clang-format -i $(SRC_DIR)/*.cpp \
		$(SRC_DIR)/dde/*.h $(SRC_DIR)/dde/*.cpp \
		$(TARGETS_DIR)/*.cpp \
		tests/integration/*.cpp \
		$(TARGETS_DIR)/*.h \
		$(LIB_DIR)/dde/include/*.h \
		$(LIB_DIR)/cppunitlite/*.h $(LIB_DIR)/cppunitlite/*.cpp \
		&& echo "Formatted C++ project"
	@black scripts/*.py

clean:
	rm -rf *.out
	$(MAKE) -C $(SRC_DIR) clean
	$(MAKE) -C $(TARGETS_DIR) clean
	$(MAKE) -C $(TESTS_DIR) clean
	$(MAKE) -C $(LIB_DIR)/cppunitlite clean

.PHONY: clean-targets format targets tools
