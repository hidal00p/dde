TARGET = libpin.a
OBJECTS = pin.o
CPP_FLAGS := -std=c++17 -g $(INCLUDE_FLAGS)

$(TARGET): $(OBJECTS)
	ar cr $@ $^

%.o: %.cpp %.H
	g++ -c $(CPP_FLAGS) $< -o $@

format:
	clang-format -i *.H *.cpp

clean:
	rm -rf $(OBJECTS) $(TARGET)
