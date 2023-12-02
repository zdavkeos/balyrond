
all: sim_anneal # hull

hull: hull.cpp
	g++ hull.cpp -std=c++11  -I/usr/include/python3.10 -lpython3.10 -o hull
	clang++ hull.cpp -std=c++11 $(shell python3-config --cflags --ldflags) -o hull

sim_anneal: sim_anneal.cpp
	clang++ -o sim_anneal sim_anneal.cpp --std=c++11 -larmadillo -I/opt/homebrew/Cellar/ensmallen/2.21.0/include -I/opt/homebrew/Cellar/armadillo/12.6.6/include -L/opt/homebrew/Cellar/armadillo/12.6.6/lib
#	g++ -o sim_anneal sim_anneal.cpp -larmadillo

PHONY: clean

clean:
	rm -rf hull sim_anneal *.o
