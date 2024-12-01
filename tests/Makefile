SRC_INCLUDE = ../src

TEST_STACK_SRC = test_stack.cpp ../src/graph_utils.cpp
TEST_STACK_H = ../src/graph_utils.h

test_stack.exe: $(TEST_STACK_SRC) $(TEST_STACK_H) 
	@g++ -I$(SRC_INCLUDE) -o $@ $(TEST_STACK_SRC)

test_%.exe: %.o test_%.cpp
	g++ -o $@ $^

%.o: %.cpp
	g++ -c $^

clean:
	rm -rf *.o *.exe

.PHONY: clean
.PRECIOUS: %.o %.exe
