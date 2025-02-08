INT_TESTS = test_overhead.exe
INCLUDE_FLAGS = -I"../lib/dde/include" -I"../lib/utils/include"
CPP_FLAGS = -std=c++17 -fno-plt -mfpmath=387 $(INCLUDE_FLAGS)
LIB_FLAGS = -L"../lib/utils" -lmytime

int_test: $(INT_TESTS)

%.exe: integration/%.cpp
	g++ $(CPP_FLAGS) $^ $(LIB_FLAGS) -o $@

clean:
	rm -rf integration/*.o *.exe

format:
	clang-format -i integration/*.cpp

.PHONY: clean format
