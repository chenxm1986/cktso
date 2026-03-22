CKTSO----Parallel Sparse Direct Solver for Circuit Simulation
============


CKTSO is a high-performance **parallel sparse direct solver specially designed for SPICE-based circuit simulation**. CKTSO is the successor of [NICSLU](https://github.com/chenxm1986/nicslu). CKTSO uses many similar techniques to NICSLU. However, CKTSO integrates some novel techniques and shows higher performance, better scalability and less memory usage than NICSLU. The most important features of CKTSO include 
+ a new pivoting-reduction technique that significantly improves the performance and scalability of parallel LU factorization with pivoting; 
+ a new memory allocation strategy that minimizes memory usage; 
+ parallel forward and backward substitutions;
+ novel nested dissection ordering, which reduces the number of floating-point operations by up to several times for post-layout/mesh-style circuits, and it also produces fewer floating-point operations than METIS;
+ novel minimum degree ordering variants, which reduce about 30-40% floating-point operations compared with mainstream methods (e.g., approximate minimum degree);
+ an adaptive numerical kernel selection method.

CKTSO supports both real and complex matrices. Both row and column modes are supported.

CKTSO is easy to use. Only a single header file and a single shared-library are needed. No need to configure the header file or any macro definition. No additional dependency is needed. Only a few simple steps to compile and run the demos.

CKTSO has an associated **GPU acceleration** module, [CKTSO-GPU](https://github.com/chenxm1986/cktso-gpu), by using CUDA. It provides acceleration for re-factorization as well as forward and backward substitutions for slightly-dense circuit matrices.

Design Philosophy
======================
CKTSO is designed for **practical** circuit simulation problems, rather than purely doing researches or publishing papers.

The primary goal of CKTSO is **accuracy**, and then performance. Accuracy is the top priority because matrices created from some practical circuits can be quite ill-conditioned or sensitive to pivot selection. **The first-time factorization must select pivots from a full range**, to provide a good numerical pattern for subsequent factorizations. This is the reason of CKTSO factorizing a matrix as a whole instead of partitioning the matrix. BBD-based parallelization is a classical method which can improve parallel scalability and cache hit ratio. However, BBD matrices should be built from circuit partitioning, ensuring all subcircuits are solvable, but not from matrix partitioning. Due to the loss of circuit-level information, matrix partitioning can lead to singular submatrices, which will cause factorization failure. This situation has been observed in practical circuit simulations. Another disadvantage of the partitioning-based method (for both circuit- and matrix-level partitioning) is the increased fill-ins, and for some circuits/matrices the increment can be very large.

Also for this reason, CKTSO does not adopt supernode diagonal block pivoting, either, although such methods can fix the dependency graph which also significantly improves the parallel scalability.

The factorization of CKTSO adopts a thresholded partial pivoting method, which selects the pivot from the complete row, just like KLU, ensuring the maximum range of pivot selection. The refactorization of CKTSO does not perform pivoting, also like KLU. The two functions can be safely invoked based on the convergence status of Newton-Raphson iterations in circuit simulation.

However, ensuring the maximal pivoting range sacrifices parallelism, though factorization with partial pivoting is called in very few iterations. To improve parallel scalability of factorization, CKTSO uses some tricks inspired from the idea of refactorization. It seeks to maximize symbolic pattern reuse by skipping pivoting; however, it keeps checking pivots during factorization. Once an unacceptable pivot is found, it falls back to partial pivoting starting from an appropriate row to minimize the impact of bad pivots. This is called fast factorization in CKTSO, which can be a replacement of factorization. The fast factorization can achieve similar parallel scalability to refactorization most of the time.

Another trick is at the reordering step. Since numerical factorization is called many times in circuit simulation, reordering should minimize fill-ins and flops. CKTSO integrates 10 reordering methods and selects the best among them by default. Though reordering is slow, the performance of repeated factorization and solving can be optimal. This strategy is also different from general-purpose solvers.


Performance Results
============
CKTSO is extremely fast. For most circuit matrices from [SuiteSparse Matrix Collection](https://sparse.tamu.edu/), CKTSO needs only **a few to tens of milliseconds** to solve.

Compared with the popular circuit solver KLU (sequential, using approximate minimum degree ordering), CKTSO is on average *>3X faster when sequential* and *>25X faster when using 16 threads*, based on the results of 66 circuit matrices from SuiteSparse matrix collection (dimensions from 1K to 5M).

Please refer to [doc/results.pdf](https://github.com/chenxm1986/cktso/blob/master/doc/results(version202205).pdf) for the detailed performance comparisons with other solvers (including both CPU- and GPU-based solvers). On average, CKTSO is faster than KLU, NICSLU, Intel MKL PARDISO and two GPU-based sparse solvers for circuit matrices. The results of CKTSO are from an old version.

CKTSO achieves the fewest operations on average by the novel ordering methods, compared with other mainstream ordering methods such as approximate minimum degree and METIS. Please refer to [doc/ordering.pdf](https://github.com/chenxm1986/cktso/blob/master/doc/ordering(version20231101).pdf) for detailed comparisons.






Notes on Library and Integer Bitwidths
============
Only x86-64 libraries are provided. This means that, a 64-bit Windows or Linux operating system is needed.

Functions for both 32-bit integers and 64-bit integers are provided. The latter has '_L' in the function names. The integer bitwidth only limits the size of the input matrix. The internal data structures always use 64-bit integers.


Working Directions
============
I am now working on the following directions to further improve CKTSO:
+ Faster GPU acceleration method;
+ automated thread number control based on system workload (supported from 20240630);
+ handling duplicated entries in input CSR format (supported from 20231101);
+ ordering for post-layout, power grid, and mesh-like matrices (supported from 20231101).



Related Publications
============
[1] Xiaoming Chen, "CKTSO: High-Performance Parallel Sparse Linear Solver for General Circuit Simulations", IEEE Transactions on Computer-Aided Design of Integrated Circuits and Systems (IEEE TCAD), vol. 44, no. 5, pp. 1887-1900, May 2025.

[2] Xiaoming Chen, "Numerically-Stable and Highly-Scalable Parallel LU Factorization for Circuit Simulation", in 2022 International Conference On Computer Aided Design (ICCAD'22).

[3] Xiaoming Chen, Yu Wang, Huazhong Yang, "Parallel Sparse Direct Solver for Integrated Circuit Simulation", Springer Publishing, 1st edition, 2017.

[4] Xiaoming Chen, Lixue Xia, Yu Wang, Huazhong Yang, "Sparsity-Oriented Sparse Solver Design for Circuit Simulation",  Design, Automation, and Test in Europe (DATE) 2016, pages 1580–1585, 14-18 March 2016.

[5] Xiaoming Chen, Wei Wu, Yu Wang, Hao Yu, Huazhong Yang, "An EScheduler-Based Data Dependence Analysis and Task Scheduling for Parallel Circuit Simulation", Circuits and Systems II: Express Briefs, IEEE Transactions on, 58(10):702–706, oct. 2011.

[6] Xiaoming Chen, Yu Wang, Huazhong Yang, "NICSLU: An Adaptive Sparse Matrix Solver for Parallel Circuit Simulation", Computer-Aided Design of Integrated Circuits and Systems, IEEE Transactions on, 32(2):261–274, feb. 2013.

[7] Xiaoming Chen, Yu Wang, Huazhong Yang, "An adaptive LU factorization algorithm for parallel circuit simulation", Design Automation Conference (ASP-DAC), 2012 17th Asia and South Pacific, pages 359–364, jan. 30 2012-feb. 2 2012.

[8] Xiaoming Chen, Yu Wang, Huazhong Yang, "A Fast Parallel Sparse Solver for SPICE-based Circuit Simulators", Design, Automation, and Test in Europe (DATE), pages 205–210, 9-13 march 2015.

[9] Xiaoming Chen, Ling Ren, Yu Wang, Huazhong Yang, "GPU-Accelerated Sparse LU Factorization for Circuit Simulation with Performance Modeling", IEEE Transactions on Parallel and Distributed Systems (IEEE TPDS), vol.26, no.3, pp.786-795, March 2015.

[10] Ling Ren, Xiaoming Chen, Yu Wang, Chenxi Zhang, Huazhong Yang, "Sparse LU factorization for parallel circuit simulation on GPU", 2012 49th Design Automation Conference (DAC'12), pp.1125-1130, June 3-7, 2012.

Author
============
Please visit [Xiaoming Chen's personal page](http://people.ucas.edu.cn/~chenxm).
