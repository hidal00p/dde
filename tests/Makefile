UNIT_TEST_DIR = lib/cppunitlite
MOCK_LIB_DIR = lib/pinmock
INTEGRATION_DIR = ./integration
UNIT_DIR = ./unit

all: integration unit

cppunit:
	@echo "===== Making unit testing framework library ====="
	@make -C $(UNIT_TEST_DIR)
	@echo

mocklib:
	@echo "===== Making dde mock library ====="
	@make -C $(MOCK_LIB_DIR)
	@echo

unit: mocklib cppunit
	@echo "===== Making unit tests ====="
	@make -C $(UNIT_DIR)
	@echo

integration: $(INT_TESTS)
	@echo "===== Making integration tests ====="
	@make -C $(INTEGRATION_DIR)
	@echo

clean:
	@make -C $(INTEGRATION_DIR) clean
	@make -C $(UNIT_DIR) clean
	@make -C $(MOCK_LIB_DIR) clean

format:
	@make -C $(INTEGRATION_DIR) format
	@make -C $(UNIT_DIR) format
	@make -C $(MOCK_LIB_DIR) format

.PHONY: clean format all unit integration cppunit mocklib
