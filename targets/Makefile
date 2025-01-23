TARGETS = polynom.exe
CPP_FLAGS = -std=c++17 -g -mfpmath=387

all: $(TARGETS)

%.exe: %.cpp
	g++ $(CPP_FLAGS) $< -o $*.exe

clean:
	rm -rf *.exe
