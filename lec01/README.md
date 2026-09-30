# Lecture 1: Parallel Array Sum

This demo sums an array of N integers with P threads. It solves the problem
twice: once with Pthreads, where we do all the work by hand, and once with
OpenMP, where a single pragma does it. Each program first times a sequential
loop, then times the parallel version, and checks that the two sums match.

## Files

- `gen_input.cpp` - writes N random integers in [0, 100) to a binary file (fixed seed).
- `sum_pthread.cpp` - sequential sum vs. Pthreads parallel sum.
- `sum_openmp.cpp` - sequential sum vs. OpenMP parallel sum.
- `Makefile` - builds the programs, generates data, runs the experiment.

## Build and run

```
make          # build gen_input, sum_pthread, sum_openmp
make data     # create data.bin with N = 100,000,000 (about 400 MB)
make run      # run both programs with P = 1, 2, 4, 8, 16
```

To run a single program by hand:

```
./gen_input 100000000 data.bin
./sum_pthread data.bin 8
./sum_openmp data.bin 8
```

Output format:

```
N = 100000000, P = 8
Sequential: sum = 4949755456, time = 52.3 ms
Parallel:   sum = 4949755456, time = 7.6 ms
Speedup:    6.89x
Result:     CORRECT
```

Measured results from one `make run` (Intel Xeon Platinum 8468, 48 cores,
Ubuntu 24.04, g++ 13.3). Your numbers will differ.

| P  | Pthreads speedup | OpenMP speedup |
|----|------------------|----------------|
| 1  | 0.99x            | 1.01x          |
| 2  | 1.97x            | 2.00x          |
| 4  | 3.67x            | 3.82x          |
| 8  | 6.89x            | 6.65x          |
| 16 | 9.78x            | 8.77x          |

All runs printed `Result: CORRECT`. The numbers vary a little from run to run
(e.g. P = 8 ranged from 5.2x to 6.9x over three runs), so run it a few times.

## Things to observe and discuss

- **Amount of code.** Compare the parallel part of the two programs. With
  Pthreads we partition the array, pack arguments into a struct, create
  threads, join them, and combine the partial sums ourselves. OpenMP does all
  of that with one `#pragma omp parallel for reduction(+:sum)`.
- **The speedup is not linear in P.** It grows more slowly than P and levels
  off as P increases. The main reason: each element needs just one addition, so
  the cores spend most of their time waiting for data from memory. The program
  is *memory-bandwidth bound*, and adding cores does not add memory bandwidth.
  Thread creation overhead also matters, especially when N is small.
- **Why integers instead of doubles?** Floating-point addition is not
  associative: `(a + b) + c` can differ from `a + (b + c)` in the last bits.
  A parallel sum adds the numbers in a different order, so a sum of doubles
  might not match the sequential result exactly. Integer sums always match, so
  we can check correctness with `==`.

## Note for macOS

Apple's default `clang` does not support `-fopenmp`. Use a Linux machine, or
install GCC (e.g. `brew install gcc`) and set `CXX=g++-14` (or your version).
