INTEGRATION_DIR = ./integration
UNIT_DIR = ./unit

all: integration unit

unit: 
	make -C $(UNIT_DIR)

integration: $(INT_TESTS)
	make -C $(INTEGRATION_DIR)

clean:
	make -C $(INTEGRATION_DIR) clean
	make -C $(UNIT_DIR) clean

format:
	make -C $(INTEGRATION_DIR) format
	make -C $(UNIT_DIR) format

.PHONY: clean format all unit integration
