COMPILER = g++
CCFLAGS = -std=c++17 -g -ansi
OBJECTS = Test.o \
		  TestRegistry.o \
		  TestResult.o \
		  TestResultStdErr.o \
		  Success.o \
		  Failure.o

cppunitlite: $(OBJECTS)
	ar cr libcppunitlite.a $(OBJECTS)
 
%.o : %.cpp %.h
	$(COMPILER) $(CCFLAGS) -c $<

format:
	clang-format -i *.cpp *.h

clean:
	rm -rf $(OBJECTS) libcppunitlite.a

.PHONY: clean format
