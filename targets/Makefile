TARGETS = main.exe
OBJS = graph.o mylib.o time.o

# -fno-plt     - use absolute references for dynamically linked libraries
# -mfpmath=387 - use x87 instruction extensions instead of SSE for flops
CPP_FLAGS = -std=c++17 -fno-plt -mfpmath=387

all: $(TARGETS)

%.exe: %.cpp $(OBJS)
	g++ $(CPP_FLAGS) $< $(OBJS) -o $*.exe

%.o: %.cpp %.h
	g++ $(CPP_FLAGS) -c $<

clean:
	rm -rf *.exe *.ds
