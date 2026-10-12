# TCSS 570 Exercises: The Work-Span Model and Parallel Patterns

For all problems, use the bounds from lecture:

max(T₁ / p, T∞) ≤ Tₚ ≤ T₁ / p + T∞

## Problem 1 (Map)

A vector search system compares a query against a database of n = 1,000,000 vectors. Computing the distance between the query and one database vector takes 1 μs, and all distances can be computed independently.

(a) What are the work T₁, the span T∞, and the parallelism?

(b) Give a lower and an upper bound on T₁₆, the running time on 16 processors.

(c) Beyond how many processors does adding more processors stop helping, according to the model?

## Problem 2 (Reduce)

We sum n = 2²⁰ = 1,048,576 integers with a balanced binary tree. Each addition takes 1 ns.

(a) What are the work T₁, the span T∞, and the parallelism?

(b) Give a lower and an upper bound on T₈, the running time on 8 processors. What speedup does the model predict?

## Problem 3 (Stencil)

A 1D heat-diffusion simulation updates n = 10,000 grid points for T = 1,000 iterations. Each point update takes 1 ns and depends on the point and its two neighbors from the previous iteration.

(a) What are the work T₁, the span T∞, and the parallelism?

(b) Give a lower and an upper bound on T₆₄, the running time on 64 processors. What is the lower bound on the efficiency?

(c) A second simulation has the same total work but a different shape: n = 100 grid points for T = 100,000 iterations. Repeat (a) and (b) for this simulation. What is the maximum speedup it can ever reach, no matter how many processors are used?

---

## Solutions

### Problem 1

(a) The DAG has n independent vertices, each taking 1 μs.

- T₁ = 1,000,000 × 1 μs = **1,000,000 μs = 1 s**
- T∞ = **1 μs** (a single distance computation)
- Parallelism = T₁ / T∞ = **1,000,000**

(b) T₁ / p = 1,000,000 / 16 = 62,500 μs.

- Lower bound: max(62,500, 1) = **62,500 μs**
- Upper bound: 62,500 + 1 = **62,501 μs**

So T₁₆ ≈ 62.5 ms, and the speedup is essentially 16. The two bounds almost coincide because the span is negligible.

(c) By the span law, Tₚ ≥ T∞ = 1 μs for every p. This bound is reached at p = **1,000,000**, so more than one million processors cannot help.

### Problem 2

(a) A balanced binary tree with n leaves has n − 1 additions and ⌈log₂ n⌉ levels.

- T₁ = 2²⁰ − 1 = **1,048,575 ns** (≈ 1.05 ms)
- T∞ = log₂ 2²⁰ = **20 ns**
- Parallelism = 1,048,575 / 20 ≈ **52,429**

(b) T₁ / p = 1,048,575 / 8 ≈ 131,072 ns.

- Lower bound: max(131,072, 20) ≈ **131,072 ns**
- Upper bound: 131,072 + 20 ≈ **131,092 ns**

So T₈ ≈ 0.131 ms, and the predicted speedup is 1,048,575 / 131,092 ≈ **8.0**, essentially linear.

### Problem 3

(a) The DAG has T layers of n vertices. Each vertex depends on 3 vertices in the previous layer, so the longest path passes through one vertex per iteration.

- T₁ = 10,000 × 1,000 × 1 ns = **10,000,000 ns = 10 ms**
- T∞ = 1,000 × 1 ns = **1,000 ns = 1 μs**
- Parallelism = 10,000,000 / 1,000 = **10,000** (= n)

(b) T₁ / p = 10,000,000 / 64 = 156,250 ns.

- Lower bound: max(156,250, 1,000) = **156,250 ns**
- Upper bound: 156,250 + 1,000 = **157,250 ns**

Efficiency = T₁ / (p · Tₚ) ≥ 10,000,000 / (64 × 157,250) ≈ **0.99**.

(c) n = 100, T = 100,000:

- T₁ = 100 × 100,000 × 1 ns = **10,000,000 ns = 10 ms** (unchanged)
- T∞ = 100,000 × 1 ns = **100,000 ns = 0.1 ms**
- Parallelism = 10,000,000 / 100,000 = **100**

With p = 64:

- Lower bound: max(156,250, 100,000) = **156,250 ns**
- Upper bound: 156,250 + 100,000 = **256,250 ns**

The speedup is between 10,000,000 / 256,250 ≈ **39.0** and 64, and the efficiency lower bound drops to 10,000,000 / (64 × 256,250) ≈ **0.61**.

Maximum speedup for any p: T₁ / T∞ = **100**.

Takeaway: the two simulations do exactly the same amount of work, but the second one has a span 100 times longer. Work alone does not tell you how well a computation can be parallelized; the shape of the DAG, captured by the span, does.