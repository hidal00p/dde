test_%.exe: %.o test_%.cpp
	g++ -o $@ $^

%.o: %.cpp
	g++ -c $^

clean:
	rm -rf *.o *.exe

.PHONY: clean
.PRECIOUS: %.o %.exe
