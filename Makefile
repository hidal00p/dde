all:
	$(MAKE) -C src/ my_tool

format:
	clang-format -i src/*.cpp

clean:
	$(MAKE) -C src/ clean

.PHONY: format
