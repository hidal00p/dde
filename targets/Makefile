TARGETS = polynom.exe \
		  traces.exe

all: $(TARGETS)

%.exe: %.cpp
	g++ -g $< -o $*.exe

clean:
	rm -rf *.exe
