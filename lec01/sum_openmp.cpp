// Parallel array sum with OpenMP.
#include <omp.h>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

using Clock = std::chrono::steady_clock;

bool read_file(const char* path, std::vector<int32_t>& data) {
    FILE* f = fopen(path, "rb");
    if (!f) return false;
    int64_t n = 0;
    bool ok = fread(&n, sizeof(n), 1, f) == 1;
    if (ok) {
        data.resize(n);
        ok = fread(data.data(), sizeof(int32_t), n, f) == (size_t)n;
    }
    fclose(f);
    return ok;
}

double ms_since(Clock::time_point start) {
    return std::chrono::duration<double, std::milli>(Clock::now() - start).count();
}

int main(int argc, char* argv[]) {
    std::vector<int32_t> data;
    int p = argc == 3 ? atoi(argv[2]) : 0;
    if (p < 1 || !read_file(argv[1], data)) {
        printf("Usage: %s <input_file> <num_threads>\n", argv[0]);
        return 1;
    }
    int64_t n = data.size();

    // Sequential baseline.
    auto t0 = Clock::now();
    int64_t seq_sum = 0;
    for (int64_t i = 0; i < n; i++) seq_sum += data[i];
    double seq_ms = ms_since(t0);

    // Parallel version: the pragma does everything sum_pthread.cpp does by hand.
    auto t1 = Clock::now();
    int64_t par_sum = 0;
    omp_set_num_threads(p);
    // "parallel":  create a team of P threads (like pthread_create).
    // "for":       split the iterations among the threads; with GCC's default
    //              static schedule, one contiguous block each (like [t*N/P, (t+1)*N/P)).
    // "reduction(+:par_sum)": each thread adds into its own private copy of
    //              par_sum, starting at 0 (like the local variable); at the end
    //              the copies are added into par_sum (like summing partial[]).
    //              Without it, all threads would update one shared par_sum at
    //              the same time (a data race) and the result would be WRONG.
    // The implicit barrier at the end of the loop waits for all threads
    // (like pthread_join).
    #pragma omp parallel for reduction(+:par_sum)
    for (int64_t i = 0; i < n; i++) par_sum += data[i];
    double par_ms = ms_since(t1);

    printf("N = %lld, P = %d\n", (long long)n, p);
    printf("Sequential: sum = %lld, time = %.1f ms\n", (long long)seq_sum, seq_ms);
    printf("Parallel:   sum = %lld, time = %.1f ms\n", (long long)par_sum, par_ms);
    printf("Speedup:    %.2fx\n", seq_ms / par_ms);
    printf("Result:     %s\n", seq_sum == par_sum ? "CORRECT" : "WRONG");
    return 0;
}
