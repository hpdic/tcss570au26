// Weak scaling of the array sum with OpenMP: every thread always sums
// n elements, so the total problem size N = n * P grows with P.
#include <omp.h>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

using Clock = std::chrono::steady_clock;
const int REPS = 5;

double ms_since(Clock::time_point start) {
    return std::chrono::duration<double, std::milli>(Clock::now() - start).count();
}

int main(int argc, char* argv[]) {
    int64_t n = argc == 3 ? atoll(argv[1]) : 0;
    int max_p = argc == 3 ? atoi(argv[2]) : 0;
    if (n < 1 || max_p < 1) {
        printf("Usage: %s <n_per_thread> <max_threads>\n", argv[0]);
        return 1;
    }
    if (max_p > omp_get_num_procs())
        printf("Warning: %d threads but only %d cores; results are not meaningful.\n",
               max_p, omp_get_num_procs());

    printf("Weak scaling: n = %lld elements per thread, median of %d runs\n",
           (long long)n, REPS);
    printf("%5s %12s %12s %12s   %s\n", "P", "N", "Time (ms)", "Efficiency", "Result");
    double t_1 = 0;  // T(1), the median time with one thread
    for (int p = 1; p <= max_p; p *= 2) {
        int64_t N = n * p;  // weak scaling: the problem grows with P
        std::vector<int32_t> data(N);
        // Parallel init: "first touch" places each page in the memory of the
        // thread that will use it (explained in Lecture 4). Not timed.
        #pragma omp parallel for schedule(static) num_threads(p)
        for (int64_t i = 0; i < N; i++) data[i] = i % 100;

        // Expected result from a plain sequential loop (not timed).
        int64_t expected = 0;
        for (int64_t i = 0; i < N; i++) expected += data[i];

        // A single run can be disturbed by other activity on the machine,
        // so time REPS runs and report the median.
        double times[REPS];
        int64_t sum = 0;
        for (int r = 0; r < REPS; r++) {
            auto start = Clock::now();
            sum = 0;
            #pragma omp parallel for reduction(+:sum) schedule(static) num_threads(p)
            for (int64_t i = 0; i < N; i++) sum += data[i];
            times[r] = ms_since(start);
        }
        std::sort(times, times + REPS);
        double t = times[REPS / 2];
        if (p == 1) t_1 = t;

        // Weak-scaling efficiency E(P) = T(1) / T(P); ideal is 1.00 (flat time).
        printf("%5d %12lld %12.2f %12.2f   %s\n", p, (long long)N, t, t_1 / t,
               sum == expected ? "CORRECT" : "WRONG");
    }  // data goes out of scope here, freeing the memory before the next P
    return 0;
}
