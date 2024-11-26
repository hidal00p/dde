TARGETS = polynom.exe \
		  traces.exe
CPP_FLAGS = -g -m64 -mno-sse

all: $(TARGETS)

%.exe: %.cpp
	g++ $(CPP_FLAGS) $< -o $*.exe

clean:
	rm -rf *.exe
