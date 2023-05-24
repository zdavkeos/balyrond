
all: hull

hull: hull.cpp
	g++ hull.cpp -std=c++11  -I/usr/include/python3.10 -lpython3.10 -o hull

PHONY: clean

clean:
	rm -rf hull *.o
