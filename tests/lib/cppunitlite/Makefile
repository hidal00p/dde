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

clean:
	rm -rf $(OBJECTS) libcppunitlite.a
