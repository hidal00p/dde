TARGETS = polynom.exe
OBJS = graph.o
CPP_FLAGS = -std=c++17 -g -fno-plt -mfpmath=387

all: $(TARGETS)

%.exe: %.cpp $(OBJS)
	g++ $(CPP_FLAGS) $< $(OBJS) -o $*.exe

%.o: %.cpp
	g++ $(CPP_FLAGS) -c $<

clean:
	rm -rf *.exe
