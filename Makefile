TARGETS_DIR := targets
SRC_DIR := src

dde:
	$(MAKE) -C $(SRC_DIR) my_tool

targets:
	$(MAKE) -C $(TARGETS_DIR)

format:
	clang-format -i $(SRC_DIR)/*.cpp $(TARGETS_DIR)/*.cpp

clean:
	$(MAKE) -C $(SRC_DIR)/ clean
	$(MAKE) -C $(TARGETS_DIR)/ clean

.PHONY: format dde targets
