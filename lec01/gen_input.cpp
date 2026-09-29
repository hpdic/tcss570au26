// Generate N random integers in [0, 100) and write them to a binary file.
// File format: int64_t N, followed by N int32_t values.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <vector>

int main(int argc, char* argv[]) {
    if (argc != 3 || atoll(argv[1]) < 1) {
        printf("Usage: %s <N> <output_file>\n", argv[0]);
        return 1;
    }
    int64_t n = atoll(argv[1]);

    // Integers, not doubles: floating-point addition is not associative, so a
    // parallel sum of doubles may differ from the sequential sum in the last bits.
    std::mt19937 gen(42);  // fixed seed, so every run produces the same data
    std::uniform_int_distribution<int32_t> dist(0, 99);
    std::vector<int32_t> data(n);
    for (int64_t i = 0; i < n; i++) data[i] = dist(gen);

    FILE* f = fopen(argv[2], "wb");
    if (!f) {
        printf("Usage: %s <N> <output_file>\n", argv[0]);
        return 1;
    }
    fwrite(&n, sizeof(n), 1, f);
    fwrite(data.data(), sizeof(int32_t), n, f);
    fclose(f);

    printf("Wrote N = %lld integers to %s (%lld bytes)\n", (long long)n, argv[2],
           (long long)(sizeof(n) + n * sizeof(int32_t)));
    return 0;
}
