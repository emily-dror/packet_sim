.PHONY: all clean debug

all: clean
	g++ -std=c++17 -DNDEBUG -o simulator *.cpp

debug: clean
	g++ -std=c++17 -o simulator *.cpp

clean:
	rm -f *.o
	rm -f simulator
