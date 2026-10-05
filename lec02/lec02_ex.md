# TCSS 570 Lecture 2 Exercises: Amdahl's Law and Gustafson's Law

## Problem 1 (Amdahl's Law)

A program takes 100 s on one processor. Of that, 20 s is inherently sequential, and the rest is perfectly parallelizable.

(a) What are the speedup and the efficiency on 4 processors? On 16 processors?

(b) What is the maximum possible speedup, no matter how many processors you use?

## Problem 2 (Gustafson's Law)

On 64 processors, a program spends 5 s in sequential code and 95 s in parallel code, for 100 s in total. A student says: "The serial fraction is 5%, so by Amdahl's law the speedup is only about 15."

(a) Is the student right? What is the actual speedup?

(b) How long would the same workload take on a single processor?

(c) What is the parallel efficiency?

---

## Solutions

### Problem 1

The serial fraction is f = 20 / 100 = 0.2.

(a) S(p) = 1 / (f + (1 − f) / p)

- S(4) = 1 / (0.2 + 0.8 / 4) = 1 / 0.4 = **2.5**
- S(16) = 1 / (0.2 + 0.8 / 16) = 1 / 0.25 = **4**

Efficiency E(p) = S(p) / p:

- E(4) = 2.5 / 4 = **0.625** (62.5%)
- E(16) = 4 / 16 = **0.25** (25%)

(b) S(∞) = 1 / f = **5**

Going from 4 to 16 processors (4× more) raises the speedup only from 2.5 to 4, and the efficiency drops from 62.5% to 25%: three quarters of the 16 processors' capacity is wasted.

### Problem 2

(a) The student is wrong. The 5% is the serial share of the **parallel** run time, not of the single-processor run time, so Gustafson's law applies:

S(p) = s + p (1 − s) = 0.05 + 64 × 0.95 = 0.05 + 60.8 = **60.85**

Misapplying Amdahl's law with f = 0.05 gives 1 / (0.05 + 0.95 / 64) ≈ 15.42, which is incorrect here.

(b) On one processor, the sequential part still takes 5 s. The parallel part, done by 64 processors in 95 s, would take 95 × 64 = 6080 s on a single processor.

T₁ = 5 + 6080 = **6085 s**, so S = 6085 / 100 = 60.85, which confirms (a).

(c) E = 60.85 / 64 ≈ **0.95** (95%). Under the student's Amdahl reading, it would be 15.42 / 64 ≈ 0.24 (24%).

Takeaway: the two laws define the "serial fraction" relative to different baselines. In Amdahl's law it is the share of the single-processor time; in Gustafson's law it is the share of the parallel time.