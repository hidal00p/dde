INT_TESTS := test_overhead.exe
INCLUDE_FLAGS := -I"../../lib/dde/include" -I"../../lib/utils/include"
CPP_FLAGS := -std=c++17 -fno-plt -mfpmath=387 $(INCLUDE_FLAGS)
LIB_FLAGS := -L"../../lib/utils" -lmytime

all: $(INT_TESTS)

%.exe: %.cpp
	g++ $(CPP_FLAGS) $^ $(LIB_FLAGS) -o $@

clean:
	rm -rf *.o *.exe

format:
	clang-format -i *.cpp

.PHONY: clean format

