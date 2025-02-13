UNIT_TESTS = test_all.exe
INCLUDE_FLAGS = -I"../../lib"
CPP_FLAGS = -std=c++17 -fno-plt -mfpmath=387 $(INCLUDE_FLAGS)
LIB_FLAGS = -L"../../lib/cppunitlite" -lcppunitlite

all: $(UNIT_TESTS)

%.exe: %.cpp
	g++ $(CPP_FLAGS) $^ $(LIB_FLAGS) -o $@

clean:
	rm -rf *.o *.exe

format:
	clang-format -i *.cpp

.PHONY: clean format
