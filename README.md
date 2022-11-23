CKTSO----Parallel Sparse Direct Solver for Circuit Simulation
============


CKTSO is a high-performance **parallel sparse direct solver specially designed for SPICE-based circuit simulation**. CKTSO is the successor of [NICSLU](https://github.com/chenxm1986/nicslu). CKTSO uses many similar techniques to NICSLU. However, CKTSO integrates some novel techniques and shows higher performance, better scalability and less memory usage than NICSLU, while NICSLU provides more functionalities. The most important new features of CKTSO include 
+ a new pivoting-reduction technique that significantly improves the performance and scalability of LU factorization with pivoting; 
+ a new memory allocation strategy that reduces memory usage; 
+ parallel forward/backward substitutions;
+ novel matrix ordering methods, which reduce about 30-40% floating-point operations compared with mainstream methods;
+ an adaptive numerical kernel selection method.

CKTSO has an associated **GPU acceleration** module, [CKTSO-GPU](https://github.com/chenxm1986/cktso-gpu), by using CUDA. It provides acceleration for re-factorization as well as forward and backward substitutions for slightly-dense circuit matrices.

Please refer to [doc/results.pdf](https://github.com/chenxm1986/cktso/blob/master/doc/results.pdf) for the results of CKTSO and comparisons with other solvers (including both CPU- and GPU-based solvers). CKTSO is faster than KLU, NICSLU, Intel MKL PARDISO and two GPU-based sparse solvers for circuit matrices.


Publications
============
[1] Xiaoming Chen, "Numerically-Stable and Highly-Scalable Parallel LU Factorization for Circuit Simulation", in 2022 International Conference On Computer Aided Design (ICCAD'22).

Author
============
Please visit [Xiaoming Chen's personal page](http://people.ucas.edu.cn/~chenxm).
