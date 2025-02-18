UNIT_TESTS := test_graph.to \
			  test_transform_ctx.to \
			  test_handlers.to

DDE_DIR := ../../src/dde
DDE_DEPS := graph.o transform_ctx.o handlers.o params.o

INCLUDE_FLAGS := -I"../lib" -I"$(DDE_DIR)"
CPP_FLAGS := -std=c++17 -g -DTEST_MODE -DTARGET_IA32E -fexceptions $(INCLUDE_FLAGS)
LIB_FLAGS := -L"../lib/cppunitlite" -lcppunitlite \
			 -L"../lib/pinmock" -lpin \
			 -L"../lib/testutils" -ltestutils
TEST_INCLUDES := -I"../lib/pinmock"

test_all.exe: test_all.cpp $(UNIT_TESTS) $(DDE_DEPS)
	g++ $(CPP_FLAGS) $^ $(LIB_FLAGS) -o $@

test_graph.to: test_graph.cpp graph.o 
	g++ -c $(CPP_FLAGS) $< -o $@

graph.o: $(DDE_DIR)/graph.cpp $(DDE_DIR)/graph.h
	g++ -c $(CPP_FLAGS) $< -o $@

test_transform_ctx.to: test_transform_ctx.cpp transform_ctx.o 
	g++ -c $(CPP_FLAGS) $(TEST_INCLUDES) $< -o $@

transform_ctx.o: $(DDE_DIR)/transform_ctx.cpp $(DDE_DIR)/transform_ctx.h
	g++ -c $(CPP_FLAGS) $(TEST_INCLUDES) $< -o $@

test_handlers.to: test_handlers.cpp handlers.o params.o
	g++ -c $(CPP_FLAGS) $(TEST_INCLUDES) $< -o $@

handlers.o: $(DDE_DIR)/handlers.cpp $(DDE_DIR)/handlers.h
	g++ -c $(CPP_FLAGS) $(TEST_INCLUDES) $< -o $@

params.o: $(DDE_DIR)/params.cpp $(DDE_DIR)/params.h
	g++ -c $(CPP_FLAGS) $(TEST_INCLUDES) $< -o $@

clean:
	rm -rf *.o *.exe *.to

format:
	clang-format -i *.cpp

.PHONY: clean format
