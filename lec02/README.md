# Lecture 2: Weak Scaling of the Array Sum

The lec01 demo was *strong scaling*: the array size N stayed fixed while the
number of threads P grew. This demo is *weak scaling*: every thread always sums
the same number of elements n, so the total size N = n * P grows with P.
Compare its results with the strong-scaling results in [../lec01](../lec01).

## Build and run

```
make                        # build sum_weak
make run                    # n = 4,000,000 per thread, P = 1, 2, 4, 8
make run MAX_THREADS=4      # e.g. on a 4-core laptop
make run N_PER_THREAD=10000 # a much smaller n per thread
```

Or run it by hand: `./sum_weak <n_per_thread> <max_threads>`. P doubles from 1
up to `max_threads`. Set `MAX_THREADS` to at most the number of **physical
cores** on your machine.

## How to read the output

```
Weak scaling: n = 4000000 elements per thread, median of 5 runs
    P            N    Time (ms)   Efficiency   Result
    1      4000000         1.37         1.00   CORRECT
    2      8000000         1.48         0.92   CORRECT
    4     16000000         1.58         0.87   CORRECT
    8     32000000         2.85         0.48   CORRECT
```

(One `make run` on an Intel Xeon Platinum 8468 virtual machine with 48 cores,
g++ 13.3. Your numbers will differ, and they also vary from run to run.)

Each row is the median time of 5 runs of the parallel sum. The weak-scaling
efficiency is E(P) = T(1) / T(P). In the ideal case every thread does the same
amount of work no matter how many threads there are, so the run time stays
flat and the efficiency stays at 1.00.

## Note for macOS

Apple's default `clang` does not support `-fopenmp`. Use a Linux machine, or
install GCC (e.g. `brew install gcc`) and run `make CXX=g++-14` (or your version).
