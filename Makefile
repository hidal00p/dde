TARGETS_DIR := targets
SRC_DIR := src

dde:
	$(MAKE) -C $(SRC_DIR) my_tool

all: dde targets

targets:
	$(MAKE) -C $(TARGETS_DIR)

format:
	clang-format -i $(SRC_DIR)/*.cpp $(TARGETS_DIR)/*.cpp

clean:
	rm -rf *.out
	$(MAKE) -C $(SRC_DIR)/ clean
	$(MAKE) -C $(TARGETS_DIR)/ clean

.PHONY: format dde targets
