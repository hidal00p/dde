TARGETS = call.exe
OBJS = 
CPP_FLAGS = -std=c++17 -g -fno-plt -mfpmath=387

all: $(TARGETS)

%.exe: %.cpp $(OBJS)
	g++ $(CPP_FLAGS) $< $(OBJS) -o $*.exe

%.o: %.cpp
	g++ -c $<

clean:
	rm -rf *.exe
