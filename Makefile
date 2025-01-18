TARGETS_DIR := targets
SRC_DIR := src
LIB_DIR := lib

all: tools targets

tools:
	$(MAKE) -C $(SRC_DIR) tools

targets:
	$(MAKE) -C $(TARGETS_DIR)

format:
	clang-format -i $(SRC_DIR)/*.cpp $(SRC_DIR)/*.h $(TARGETS_DIR)/*.cpp $(LIB_DIR)/*.h
	black scripts/*.py

clean:
	rm -rf *.out
	$(MAKE) -C $(SRC_DIR)/ clean
	$(MAKE) -C $(TARGETS_DIR)/ clean

.PHONY: format targets tools
