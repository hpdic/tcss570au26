// Parallel array sum with Pthreads.
#include <pthread.h>
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

// Everything a thread needs to know about its share of the work.
struct ThreadArgs {
    int id;               // thread id t, 0 .. P-1
    const int32_t* data;  // shared input; const: threads only read it
    int64_t n;            // total number of elements N
    int p;                // number of threads P
    int64_t* partial;     // shared output array; thread t writes only partial[t]
};

// Thread function. Pthreads requires the signature void* f(void*), so the
// argument arrives as void* and is cast back to ThreadArgs*.
void* worker(void* arg) {
    ThreadArgs* a = (ThreadArgs*)arg;
    // Block partitioning: thread t handles [t*N/P, (t+1)*N/P).
    int64_t begin = a->id * a->n / a->p;
    int64_t end = (a->id + 1) * a->n / a->p;
    // Accumulate locally and write partial[id] once: avoids false sharing.
    int64_t local = 0;
    for (int64_t i = begin; i < end; i++) local += a->data[i];
    a->partial[a->id] = local;
    return nullptr;
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

    // Parallel version: timing includes thread creation and joining.
    auto t1 = Clock::now();
    std::vector<pthread_t> threads(p);
    std::vector<ThreadArgs> args(p);
    std::vector<int64_t> partial(p);
    for (int t = 0; t < p; t++) {
        // Each thread gets its own args[t]; sharing one struct would be a race,
        // since the loop would overwrite it while threads are still reading it.
        args[t] = {t, data.data(), n, p, partial.data()};
        // Start a new thread running worker(&args[t]). Arguments: where to
        // store the thread handle, attributes (nullptr = defaults), the
        // function to run, and the one pointer passed to it.
        pthread_create(&threads[t], nullptr, worker, &args[t]);
    }
    // Join: wait for every thread to finish, then combine the partial sums.
    int64_t par_sum = 0;
    for (int t = 0; t < p; t++) {
        pthread_join(threads[t], nullptr);
        par_sum += partial[t];
    }
    double par_ms = ms_since(t1);

    printf("N = %lld, P = %d\n", (long long)n, p);
    printf("Sequential: sum = %lld, time = %.1f ms\n", (long long)seq_sum, seq_ms);
    printf("Parallel:   sum = %lld, time = %.1f ms\n", (long long)par_sum, par_ms);
    printf("Speedup:    %.2fx\n", seq_ms / par_ms);
    printf("Result:     %s\n", seq_sum == par_sum ? "CORRECT" : "WRONG");
    return 0;
}
