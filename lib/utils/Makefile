MY_LIBS = libgraph.a libmytime.a
INCLUDE_FLAGS = -I"./include"
CPP_FLAGS = -g -std=c++20 $(INCLUDE_FLAGS)

libs: $(MY_LIBS)

lib%.a: src/%.o
	ar rcs $@ $^

src/%.o: src/%.cpp include/%.h
	g++ $(CPP_FLAGS) -c $< -o $@

clean:
	rm -rf ./*.a ./src/*.o

.PHONY: clean
