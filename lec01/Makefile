CXX      = g++
CXXFLAGS = -O2 -std=c++17 -Wall

all: gen_input sum_pthread sum_openmp

gen_input: gen_input.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

sum_pthread: sum_pthread.cpp
	$(CXX) $(CXXFLAGS) -pthread -o $@ $<

sum_openmp: sum_openmp.cpp
	$(CXX) $(CXXFLAGS) -fopenmp -o $@ $<

data: gen_input
	./gen_input 100000000 data.bin

run: sum_pthread sum_openmp
	for p in 1 2 4 8 16; do \
		echo "=== Pthreads, P = $$p ==="; ./sum_pthread data.bin $$p; \
		echo "=== OpenMP,   P = $$p ==="; ./sum_openmp data.bin $$p; \
	done

clean:
	rm -f gen_input sum_pthread sum_openmp data.bin

.PHONY: all data run clean
