TARGETS_DIR := targets
SRC_DIR := src
LIB_DIR := lib

all: tools targets

tools:
	$(MAKE) -C $(SRC_DIR) tools

targets:
	$(MAKE) -C $(TARGETS_DIR)

format:
	@clang-format -i $(SRC_DIR)/*.cpp \
		$(SRC_DIR)/dde/*.h $(SRC_DIR)/dde/*.cpp \
		$(TARGETS_DIR)/*.cpp \
		tests/integration/*.cpp \
		$(TARGETS_DIR)/*.h \
		$(LIB_DIR)/dde/include/*.h \
		&& echo "Formatted C++ project"
	@black scripts/*.py

clean:
	rm -rf *.out
	$(MAKE) -C $(SRC_DIR)/ clean
	$(MAKE) clean-targets

clean-targets:
	$(MAKE) -C $(TARGETS_DIR)/ clean

.PHONY: clean-targets format targets tools
