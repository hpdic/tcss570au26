CXX      = g++
CXXFLAGS = -O2 -std=c++17 -Wall -fopenmp

# Defaults for "make run": 4M ints (16 MB) per thread, up to 8 threads.
# Override on the command line, e.g.  make run MAX_THREADS=4
N_PER_THREAD = 4000000
MAX_THREADS  = 8

all: sum_weak

sum_weak: sum_weak.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

run: sum_weak
	./sum_weak $(N_PER_THREAD) $(MAX_THREADS)

clean:
	rm -f sum_weak

.PHONY: all run clean
