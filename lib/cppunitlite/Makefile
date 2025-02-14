COMPILER = g++
CCFLAGS = -std=c++17 -g -ansi

%.o : %.cpp
	$(COMPILER) $(CCFLAGS) -c $<

objects = Test.o TestRegistry.o TestResult.o TestResultStdErr.o Success.o Failure.o

all: cppunitlite

cppunitlite: $(objects)
	ar cr libcppunitlite.a $(objects)
    
Failure.o: Failure.cpp Failure.h
Success.o: Success.cpp Success.h
Test.o: Test.cpp Test.h
TestRegistry.o: TestRegistry.cpp TestRegistry.h
TestResult.o: TestResult.cpp TestResult.h
TestResultStdErr.o: TestResultStdErr.cpp TestResultStdErr.h


clean:
	rm -rf $(objects) libcppunitlite.a
