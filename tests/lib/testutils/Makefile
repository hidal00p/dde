TARGET = libtestutils.a
OBJECTS = exceptions.o
CPP_FLAGS := -std=c++17 -g -fexceptions $(INCLUDE_FLAGS)

$(TARGET): $(OBJECTS)
	ar cr $@ $^

%.o: %.cpp %.h
	g++ -c $(CPP_FLAGS) $< -o $@

format:
	clang-format -i *.h *.cpp

clean:
	rm -rf $(OBJECTS) $(TARGET)
