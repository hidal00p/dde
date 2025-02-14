UNIT_TESTS := test_graph.to

DDE_DIR := ../../src/dde
DDE_DEPS := graph.o

INCLUDE_FLAGS := -I"../../lib" -I"$(DDE_DIR)"
CPP_FLAGS := -std=c++17 -g $(INCLUDE_FLAGS)
LIB_FLAGS := -L"../../lib/cppunitlite" -lcppunitlite

test_all.exe: test_all.cpp $(UNIT_TESTS) $(DDE_DEPS)
	g++ $(CPP_FLAGS) $^ $(LIB_FLAGS) -o $@

%.to: %.cpp
	g++ -c $(CPP_FLAGS) $^ -o $@

%.o: $(DDE_DIR)/%.cpp
	g++ -c $(CPP_FLAGS) $^ -o $@

clean:
	rm -rf *.o *.exe *.to

format:
	clang-format -i *.cpp

.PHONY: clean format
