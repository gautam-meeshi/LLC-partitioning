# About
- This repository contains the implementation of following last level cache (LLC) partitioning algorithms.
- The partitioning is done in Ruby memory subsystem on MESI three level cache coherence.

# Space partitioning of LLC
## REquest Arbitration in Last Level Cache (REAL)
[link to paper](https://dl.acm.org/doi/10.1145/3362100)
### Problem Statement
- In a multi-core environment the bandwidth at LLC queue can be partitioned between different cores deterministically to improve net instructions per clock cycle (IPC).
- The paper considers 3 parameters to implement bandwidth partitioning at last level cache (LLC).
    1. Instruction between subsequent LLC accesses (ISL): A core with higher ISL should be given higher priority at the LLC because servicing that request will unblock that core and allow it to execute faster, improving the net IPC. Relative ISL value (ISL/GM(ISL)) is used to index in the priority array and obtain an integer value of priority.
    2. Instruction fetch rate at LLC (IFR): A core accessing LLC for fetching instructions should be provided higher priority because instructions are more critical to the IPC that data. A threshold (IFR_TH) is used to determine whether the priority should be incremented.
    3. Instruction fetch miss rate at LLC: A core whose instruction fetches at LLC misses more frequently does not deserve a higher priority. A threshold is used to determine whether the IMR is high.

    If IFR > IFR_TH and IMR < IMR_TH then the priority value of the core is incremented by 1.

- The bandwidth is partitioned based on the priority vector and the partition is valid for a fixed duration of clock cycles (1.4M).The ideal value of interval is found emprically. In this interval the counters are incremented and considered for the calculation of priority for the next interval.

# Space partitioning of LLC
- Space partitioning of LLC involves with distributing the ways of the cache to cores to optimize the system performance. Following are the four cache space partitioning algorithms implemented on the simulator.

## Utility based Cache partitioning (UCP)
[link to paper](https://ieeexplore.ieee.org/document/4041865)
- It replaces demand-based allocation (LRU) by assigning cache ways to competing applications based on the reduction in cache misses they will obtain per allocated way.
- Partitioning algorithm:
1. **Utility Monitoring (UMON-DSS):** Tracks miss counts across cache ways for each application independently using Dynamic Set Sampling (ATDs monitoring 32 sampled sets)[cite: 1].
2. **Lookahead Algorithm (Scalable Search):** To prevent getting stuck in local optima with non-convex utility curves (where marginal gains require allocating multiple ways at once), UCP evaluates **Marginal Utility (MU)** over variable way-allocations:
   $$\text{MU}_{a}^{b} = \frac{\text{miss}_a - \text{miss}_b}{b - a}$$
3. **Allocation:** Iteratively assigns the batch of ways that yields the highest max marginal utility ($\mathrm{max\_mu}$) across applications until all $N$ ways are allocated, achieving optimal or near-optimal partitions in $O(N^2)$.

## Critical-Phase Aware (CPA) Cache Partitioning

The Critical-Phase Aware (CPA) partitioning scheme dynamically manages shared Last Level Cache (LLC) resources using Intel Resource Director Technology (RDT). It maximizes system throughput by prioritizing memory-sensitive critical applications while isolating cache-hogging workloads.

---

### Core Principles

* **Real-Time Monitoring:** Continuously tracks hardware counters per application to measure:
  * **IPC** (Instructions Per Cycle)
  * **MPKI** (Misses Per Kilo-Instruction)
  * **HPKI** (Hits Per Kilo-Instruction)
  * **LLC Occupancy**
* **Phase-Change Detection:** Calculates runtime performance variance (ICOV) to detect application phase transitions and trigger dynamic re-partitioning only when needed.

---

### Application Classification

Based on metric thresholds, applications are dynamically grouped into distinct behavioral profiles:

| Profile | Characteristics | Allocation Strategy |
| :--- | :--- | :--- |
| **Sensitive** | High performance dependency & high hit rate | Allocated dedicated private cache ways (High Priority) |
| **Medium** | High performance, but consumes excessive cache | Assigned restricted private ways to prevent waste |
| **Bully / Squanderer** | High miss rates, low cache utilization / high thrashing | Constrained to restricted or shared ways to protect others |
| **Non-Critical** | Low sensitivity to cache size adjustments | Grouped into a default shared baseline pool |

---

### Dynamic Way Allocation Pipeline

1. **Metrics Collection:** Gather IPC, MPKI, HPKI, and occupancy per execution interval.
2. **Phase Evaluation:** Re-classify applications if IPC variance exceeds defined thresholds.
3. **CLOS Assignment:** Map workloads to Intel RDT Class of Service (CLOS) bitmasks based on their profile.
4. **Way Optimization:** Throttle cache-hogging applications and reassign freed cache ways to critical workloads.

## XChange: A market-based approach to scalable dynamic multi-resource allocation in multicore architectures
[link to paper](https://ieeexplore.ieee.org/document/7056026)
### Key Features & Contributions

* **Market-Based Resource Allocation:** Operates as a dynamic market where shared resources are assigned prices based on supply and demand. Cores act as price-taking agents that place bids to maximize their individual utility given current resource prices.
* **Scalable & Distributed Decision-Making:** Instead of relying on a centralized search across a massive multi-resource space, decision-making is delegated to individual cores. Global overhead is restricted to a simple pricing update step.
* **High Performance & Fairness:** Delivers high system throughput while maintaining balanced, unbiased allocation across applications without sacrificing individual core progress.
* **Fully Dynamic (No Prior Profiling):** Operates at run-time without requiring prior knowledge or offline profiling of workload characteristics[cite: 1]. Cores dynamically monitor and model their own resource-performance behavior.
* **Wealth Redistribution:** Offers an optional heuristic that adjusts core budgets according to an application's potential speedup, maximizing throughput while preserving fairness bounds.

---

### How It Works

1. **Local Utility Modeling:** Each core dynamically models its execution time by decomposing work into compute and memory phases:
   * **Cache Utility:** Combines Utility-Based Cache Expansion (UMON) tag sampling with global critical path counters ($CP_{global}$) to estimate memory phase execution time across different cache way allocations.
   * **Power Utility:** Models compute phase scaling relative to per-core voltage and frequency adjustments.
2. **Guided Hill-Climbing Bidding Strategy:** Cores run a local search algorithm to determine optimal bids for cache and power under current market prices[cite: 1]. To handle non-convex cache utility curves (e.g., step-like performance gains), the algorithm begins searching from the maximum affordable cache allocation.
3. **Price Discovery & Convergence:**
   * A master arbiter collects core bids and updates resource prices ($p_j = \frac{\sum b_{ij}}{R_j}$).
   * Price smoothing ($p_j = \alpha \cdot p_j^{last} + (1-\alpha) \cdot p_j^{new}$) prevents price oscillations caused by discrete cache allocations[cite: 1].
   * Resources are allocated proportionally based on final bids ($r_{ij} = \frac{b_{ij}}{p_j}$).

## Machine Learned Machines (MLM): Algorithm Summary

Machine Learned Machines (MLM) is an online Reinforcement Learning (RL) framework designed for multi-core systems to dynamically co-optimize Last Level Cache (LLC) partitioning alongside core and uncore Dynamic Voltage and Frequency Scaling (DVFS).

---

### Core Algorithm Components

* **Dynamic Cache Partitioning MDP (LLC-MDP):**
  * **State Space:** Formed by quantizing the Time Per Instruction ratio ($TPI_{Curr}/TPI_{Prev}$) into 2 states per core. For a 4-core system, this creates a $2^4 = 16$-state space.
  * **Action Space:** A vector representing allocation ($A$), no-change ($N$), or deallocation ($D$) requests per core. For a $C$-core system, valid action combinations are constrained to maintain fixed shared cache ways and avoid fragmentation.
  * **Reward Function:** Uses an **off-chip DRAM bandwidth minimization metric**, which yielded the best performance compared to TPI or Energy Per Instruction (EPI) reward models.

* **Core DVFS MDP:**
  * **State Space:** Formed by quantizing $TPI_{Curr}/TPI_{Prev}$ into 3 performance states.
  * **Action Space:** Three discrete frequency adjustments per state: Increase ($I$), No Change ($N$), or Decrease ($D$).
  * **Shared Learning (SL):** Core agents share a single common Q-table. This accelerates convergence speed and keeps memory overhead independent of core count.

* **Uncore DVFS MDP:**
  * Operates independently to scale the frequency and voltage of the interconnect (NoC) and LLC based on uncore traffic demands.

---

### Reinforcement Learning Mechanism

Each agent updates its action-value function via one-step **Q-learning**:

$$Q(s_t, a_t) \leftarrow Q(s_t, a_t) + \alpha \left[ r_{t+1} + \gamma \max_a Q(s_{t+1}, a) - Q(s_t, a_t) \right]$$

* **Execution Interval:** Evaluated periodically (e.g., every 500 $\mu$s interval)[cite: 2]. At interval boundaries, agents measure system states, calculate rewards, update Q-tables, and execute actions for the next phase.

---

### Key Performance Results

* **Energy-Delay Product (EDP):** Achieves an average **19.6% reduction** in EDP compared to unoptimized baselines.
* **Execution Time:** Delivers an average **2.6% improvement** in overall execution time[cite: 2].
* **Overhead Savings:** The shared-learning TPI-based model consumes **87% less storage overhead** compared to prior core-DVFS RL approaches.