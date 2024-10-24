TARGETS = polynom.exe \
		  traces.exe

all: $(TARGETS)

%.exe: %.cpp
	g++ $< -o $*.exe

clean:
	rm -rf *.exe
