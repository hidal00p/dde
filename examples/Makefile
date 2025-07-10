TARGETS = newton.exe \
		  rosenbrock.exe
OBJS = mylib.o

INCLUDE_FLAGS = -I"../lib/dde/include" -I"../lib/utils/include"
CPP_FLAGS = -g -O0 -std=c++20 -fno-plt $(INCLUDE_FLAGS)
LIB_FLAGS = -L"../lib/utils" -lgraph -lmytime

all: $(TARGETS)

%.exe: %.cpp $(OBJS)
	g++ $(CPP_FLAGS) $< $(OBJS) $(LIB_FLAGS) -o $*.exe

%.o: %.cpp %.h
	g++ $(CPP_FLAGS) -c $<

format:
	@clang-format -i *.cpp *.h

clean:
	rm -rf *.exe *.ds

.PHONY: clean format
