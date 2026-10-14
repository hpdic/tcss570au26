# TCSS 570 Exercise: Drawing a Roofline

A machine has peak compute **P = 2,000 GFLOP/s** and memory bandwidth **B = 100 GB/s**.

Consider four kernels. All data are single-precision floats (4 bytes). Assume every array or matrix element is moved between memory and the processor exactly once.

| Kernel | Code / description |
|---|---|
| A: vector add | `c[i] = a[i] + b[i]` for i = 0, …, n−1 |
| B: dot product | `s += x[i] * y[i]` for i = 0, …, n−1 |
| C: matrix multiplication, n = 60 | C = A × B, all matrices n × n |
| D: matrix multiplication, n = 600 | C = A × B, all matrices n × n |

(a) Compute the ridge point I\* of the machine.

(b) For each kernel, compute the FLOPs, the bytes moved, and the arithmetic intensity I.

(c) Draw the roofline of the machine on log-log axes (x: arithmetic intensity in FLOP/byte, y: attainable performance in GFLOP/s), and place all four kernels on it.

(d) For each kernel, state whether it is memory-bound or compute-bound, and give its attainable performance and its percentage of peak.

(e) You can upgrade the machine in one of two ways: double the number of cores (P = 4,000 GFLOP/s, B unchanged), or double the memory bandwidth (B = 200 GB/s, P unchanged). Which kernels get faster under each upgrade?

---

## Solutions

### (a)

I\* = P / B = 2,000 / 100 = **20 FLOP/byte**

### (b)

**A: vector add.** Per element: 1 addition; read a[i] and b[i], write c[i], so 3 × 4 = 12 bytes.
I = 1 / 12 ≈ **0.083 FLOP/byte**

**B: dot product.** Per element: 1 multiplication + 1 addition = 2 FLOPs; read x[i] and y[i], so 8 bytes (s stays in a register).
I = 2 / 8 = **0.25 FLOP/byte**

**C and D: matrix multiplication.** FLOPs: 2n³. Bytes: read A and B, write C, so 3n² × 4 = 12n². I = 2n³ / 12n² = n / 6.
- C (n = 60): I = 60 / 6 = **10 FLOP/byte**
- D (n = 600): I = 600 / 6 = **100 FLOP/byte**

### (c)

The roofline is min(2,000, 100 × I): a line of slope 1 on log-log axes up to the ridge point I\* = 20, then flat at 2,000 GFLOP/s.

```
 GFLOP/s (log scale)
  2000 |                               --------D------
       |                            ///|
  1000 |                           C   |
       |                        ///    |
       |                       /       |
       |                    ///        |
       |                   /           |
       |                ///            |
   100 |               /               |
       |            ///                |
       |          //                   |
       |        B/                     |
       |       /                       |
       |    ///                        |
    10 |   /                           |
       | /A                            |
       |/                              |
       +---+-----------+-----------+---+-------+------
          0.1          1          10  20      100
                    I (FLOP/byte, log scale)
```

(Drawn approximately to scale on log-log axes. A, B, and C are on the slanted part of the roof; D is on the flat part. The vertical line marks the ridge point I\* = 20. Each point shows the kernel's upper bound; real measured performance can be anywhere below it.)

Key points to draw:

| Point | I (FLOP/byte) | Performance (GFLOP/s) |
|---|---|---|
| Ridge point | 20 | 2,000 |
| A: vector add | 0.083 | 8.3 |
| B: dot product | 0.25 | 25 |
| C: matmul, n = 60 | 10 | 1,000 |
| D: matmul, n = 600 | 100 | 2,000 |

A, B, and C lie on the slanted line (memory-bound); D lies on the flat roof (compute-bound).

### (d)

Attainable performance = min(P, I × B).

| Kernel | I (FLOP/byte) | Bound | Attainable (GFLOP/s) | % of peak |
|---|---|---|---|---|
| A: vector add | 0.083 | memory | 0.083 × 100 ≈ **8.3** | 0.4% |
| B: dot product | 0.25 | memory | 0.25 × 100 = **25** | 1.25% |
| C: matmul, n = 60 | 10 | memory | 10 × 100 = **1,000** | 50% |
| D: matmul, n = 600 | 100 | compute | **2,000** | 100% |

C and D are the same algorithm, but C is too small to reach the ridge point. Arithmetic intensity depends on the problem size, not just on the algorithm.

### (e)

**Double the cores (P = 4,000, B = 100).** The ridge point moves to 40.
- A, B, C: unchanged. They are memory-bound, and the slanted part of the roof does not move.
- D: 2,000 → **4,000 GFLOP/s**. Only the compute-bound kernel benefits.

**Double the bandwidth (P = 2,000, B = 200).** The ridge point moves to 10.
- A: 8.3 → **16.7 GFLOP/s**
- B: 25 → **50 GFLOP/s**
- C: 1,000 → **2,000 GFLOP/s** (now exactly at the ridge point, i.e., compute-bound)
- D: unchanged at 2,000 GFLOP/s.

Takeaway: adding cores helps only compute-bound kernels; adding bandwidth helps only memory-bound kernels. Before buying hardware or adding threads, find out where your program sits on the roofline.