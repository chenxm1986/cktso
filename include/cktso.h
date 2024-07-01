/*
* CKTSO is a high-performance parallel linear system solver for SPICE-based circuit simulations.
*/

/*
* version 20240630
*/

#ifndef __CKTSO__
#define __CKTSO__
#include <stddef.h>

/********** error code **********
*  0:   successful
* -1:   invalid instance handle
* -2:   argument error
* -3:   invalid matrix
* -4:   out of memory
* -5:   structurally singular
* -6:   numerically singular
* -7:   threads error
* -8:   matrix not analyzed
* -9:   matrix not factorized
* -10:  not supported
* -11:  file error
* -12:  integer overflow
* -13:  resource leak
* -99:  license error
* -100: unknown error
********************************/

/********** input parameters int iparm[] **********
* iparm[0]:  timer. [default 0]: no timer | >0: microsecond/us-level timer | <0: millisecond/ms-level timer
* iparm[1]:  pivoting tolerance (in millionth). [default 1000 (=0.001)]
* iparm[2]:  ordering method. [default 0]: select best from all 10 methods | 1~10: single method | 11: select best from 2 nested dissection variants 
             | 12~18: select best from (parm[2]-10) minimum degree variants | <0: no ordering
* iparm[3]:  threshold (percentage) for dense node detection in ordering. [default 1000 (=10.0)]
* iparm[4]:  metric for ordering method selection. [default >=0]: use flops | <0: use nnz
* iparm[5]:  max supernode size. [default -1]: no limitation
* iparm[6]:  minimum # of columns for supernode detection. [default 64]
* iparm[7]:  scaling. [default 0]: no scaling
* iparm[8]:  whether right-hand-vector is very sparse (e.g., # of nonzeros < n/10), only effective for sequential column-mode solve. [default 0]
* iparm[9]:  automatic thread number control based on matrix features. [default 1]
* iparm[10]: memory growth ratio (percentage). [default 150 (=1.5)]
* iparm[11]: initial # of rows for supernode creation. [default 16]
* iparm[12]: static pivoting method (only effective for CKTSO(_L)_Analyze with ax=NULL specified). 0: conventional | >0: fill-in aware | [default <0]: diagonal first
* iparm[13]: threads sync & adjust method. [default 0]: blocked wait | >0: blocked wait & dynamic thread number control based on system workload (value means time interval in 100 milliseconds for dynamic adjustment) | <0: busy wait
* iparm[14]: timeout value for waiting for slave threads to exit, in millisecond/ms. [default -1] inf (block until threads exit)
********************************/

/********** output parameters long long oparm[] **********
* oparm[0]:  time (in microsecond/us) of CKTSO(_L)_Analyze
* oparm[1]:  time (in microsecond/us) of CKTSO(_L)_Factorize or CKTSO(_L)_Refactorize
* oparm[2]:  time (in microsecond/us) of CKTSO(_L)_Solve or CKTSO(_L)_SolveMV
* oparm[3]:  time (in microsecond/us) of CKTSO(_L)_SortFactors
* oparm[4]:  # of off-diagonal pivots
* oparm[5]:  nnz(L), including diagonal
* oparm[6]:  nnz(U-I), excluding diagonal (U's diagonal elements are all 1.0)
* oparm[7]:  # of supernodes
* oparm[8]:  selected ordering method
* oparm[9]:  singular row index when -6 is returned
* oparm[10]: # of realloc invoked during factor
* oparm[11]: memory requirement (in bytes) when -4 is returned (for the last malloc/realloc failure)
* oparm[12]: current memory usage (in bytes)
* oparm[13]: maximum memory usage (in bytes)
* oparm[14]: # of rows completed with pivoting reuse in CKTSO(_L)_Factorize
* oparm[15]: time (in microsecond/us) of CKTSO(_L)_FactorizeAndSolve or CKTSO(_L)_RefactorizeAndSolve
* oparm[16]: predicted nnz(L+U-I) by symbolic analysis
* oparm[17]: predicted flops by symbolic analysis (MAC=2 flops)
********************************/

#ifndef __cplusplus
/*
* stdbool.h is added in C99. If the file does not exist, simply replace the following line with "#define bool char"
*/
#include <stdbool.h>
#endif

#undef _IN_
#define _IN_
#undef _OUT_
#define _OUT_

/*
* Instances of CKTSO, for both C and C++ interfaces
*/
typedef struct __cktso_dummy *ICktSo;
typedef struct __cktso_l_dummy *ICktSo_L;

/*
* C++ interface. Call CKTSO_CreateSolver (CKTSO_L_CreateSolver) to create solver instance first
*/
#ifdef __cplusplus
#undef _CDECL_
/*Visual C++ compiler passes 'this' pointer via ECX on Windows X86, which is different from __cdecl, so __cdecl must be declared in this case*/
#if (defined(_MSC_VER) && !defined(_M_X64) && !defined(_WIN64))
#define _CDECL_ __cdecl
#else
#define _CDECL_
#endif

struct __cktso_dummy
{
    /*
    * DestroySolver: frees everything and destroys solver instance.
    */
    virtual int _CDECL_ DestroySolver
    (
    ) = 0;

    /*
    * Analyze: analyzes matrix for static pivoting and fillin-reduction ordering. Previous matrix is first destroyed and then new matrix is created.
    * @is_complex: complex or real
    * @n: matrix dimension
    * @ap: integer array of length n+1, matrix row pointers
    * @ai: integer array of length ap[n], matrix column indexes
    * @ax: double/complex array of length ap[n], matrix values
    * @threads: # of threads to be created for analysis, factor, refactor, solve, and sort
    */
    virtual int _CDECL_ Analyze
    (
        _IN_ bool is_complex, 
        _IN_ int n,
        _IN_ const int ap[],
        _IN_ const int ai[],
        _IN_ const double ax[], /*can be NULL if unavailable when analysis*/
        _IN_ int threads /*0=use all physical cores. -1=use all logical cores*/
    ) = 0;

    /*
    * Factorize: factorizes matrix with pivoting (call this routine after matrix has been analyzed).
    * @ax: double/complex array of length ap[n], matrix values
    * @fast: whether to use fast factorization (skips symbolic, but checks pivots)
    */
    virtual int _CDECL_ Factorize
    (
        _IN_ const double ax[], 
        _IN_ bool fast
    ) = 0;

    /*
    * Refactorize: refactorizes matrix without pivoting (call this routine after matrix has been factorized with pivoting).
    * @ax: double/complex array of length ap[n], matrix values
    */
    virtual int _CDECL_ Refactorize
    (
        _IN_ const double ax[]
    ) = 0;

    /*
    * Solve: solves Ax=b after A is factorized (call this routine after matrix has been factorized or refactorized).
    * @b: double/complex array of length n to specify right-hand-side vector
    * @x: double/complex array of length n to get solution
    * @force_seq: force to use sequential solve (if not, solver automatically decides sequential or parallel)
    * @row0_column1: row or column mode
    */
    virtual int _CDECL_ Solve
    (
        _IN_ const double b[],
        _OUT_ double x[], /*x address can be same as b address for overwriting x on b*/
        _IN_ bool force_seq, 
        _IN_ bool row0_column1
    ) = 0;

    /*
    * SolveMV: solves Ax=b with multiple b after A is factorized (call this routine after matrix has been factorized or refactorized).
    * @nrhs: number of right-hand-side vectors
    * @b: double/complex array of length ld_b*nrhs to specify right-hand-side vectors, vector by vector
    * @ld_b: leading dimension of two-dimensional matrix b
    * @x: double/complex array of length ld_x*nrhs to get solutions, vector by vector
    * @ld_x: leading dimension of two-dimensional matrix x
    * @row0_column1: row or column mode
    */
    virtual int _CDECL_ SolveMV
    (
        _IN_ size_t nrhs, 
        _IN_ const double b[],
        _IN_ size_t ld_b, /*if ld_b=0, ld_b=n*/
        _OUT_ double x[], /*x address can be same as b address for overwriting x on b*/
        _IN_ size_t ld_x, /*if ld_x=0, ld_x=n*/
        _IN_ bool row0_column1
    ) = 0;

    /*
    * SortFactors: sorts indexes and/or values of LU factors (call this routine after matrix has been factorized with pivoting).
    * @also_sort_values: whether to sort values as well, false for only sorting indexes
    */
    virtual int _CDECL_ SortFactors
    (
        _IN_ bool also_sort_values /*whether to sort values as well*/
    ) = 0;

    /*
    * Statistics: calculates # of floating-point operations and memory access volumes (call this routine after matrix has been factorized with pivoting).
    * @factor_flops: pointer to a 64b integer to retrieve flops of factor
    * @solve_flops: pointer to a 64b integer to retrieve flops of solve
    * @factor_mem: pointer to a 64b integer to retrieve memory access volume of factor in bytes
    * @solver_mem: pointer to a 64b integer to retrieve memory access volume of solve in bytes
    * @row0_column1: row or column mode (affects solve_mem)
    * @scaling: whether scaling is counted in flops & memory access
    * @fuse_mac: whether multiply-and-accumulate is counted as a single fused operation
    */
    virtual int _CDECL_ Statistics
    (
        _OUT_ long long *factor_flops, /*can be NULL if not needed*/
        _OUT_ long long *solve_flops, /*can be NULL if not needed*/
        _OUT_ long long *factor_mem, /*can be NULL if not needed*/
        _OUT_ long long *solve_mem, /*can be NULL if not needed*/
        _IN_ bool row0_column1,
        _IN_ char scaling, /*0 for disabled, >0 for enabled, <0 for using solver setting*/
        _IN_ bool fuse_mac
    ) = 0;

    /*
    * CleanUpGarbage: cleans up redundant memory.
    */
    virtual int _CDECL_ CleanUpGarbage
    (
    ) = 0;

    /*
    * Determinant: calculates determinant (mantissa*10^exponent, where 1 <= abs(mantissa) < 10) (call this routine after matrix has been factorized or refactorized).
    * @mantissa: mantissa of determinant (double or complex)
    * @exponent: exponent of determinant
    */
    virtual int _CDECL_ Determinant
    (
        _OUT_ double *mantissa, 
        _OUT_ double *exponent
    ) = 0;

    /*
    * FactorizeAndSolve: factorizes matrix with pivoting and then solves Ax=b (call this routine after matrix has been analyzed).
    * @ax: double/complex array of length ap[n], matrix values
    * @b: double/complex array of length n to specify right-hand-side vector
    * @x: double/complex array of length n to get solution
    * @row0_column1: row or column mode
    */
    virtual int _CDECL_ FactorizeAndSolve
    (
        _IN_ const double ax[],
        _IN_ const double b[],
        _OUT_ double x[], /*x address can be same as b address for overwriting x on b*/
        _IN_ bool row0_column1
    ) = 0;

    /*
    * RefactorizeAndSolve: refactorizes matrix without pivoting and then solves Ax=b (call this routine after matrix has been factorized with pivoting).
    * @ax: double/complex array of length ap[n], matrix values
    * @b: double/complex array of length n to specify right-hand-side vector
    * @x: double/complex array of length n to get solution
    * @row0_column1: row or column mode
    */
    virtual int _CDECL_ RefactorizeAndSolve
    (
        _IN_ const double ax[],
        _IN_ const double b[],
        _OUT_ double x[], /*x address can be same as b address for overwriting x on b*/
        _IN_ bool row0_column1
    ) = 0;

    /*
    * Analyze2: analyzes matrix for static pivoting and fillin-reduction ordering. Previous matrix is first destroyed and then new matrix is created.
    * @is_complex: complex or real
    * @n: matrix dimension
    * @ap: integer array of length n+1, matrix row pointers
    * @ai: integer array of length ap[n], matrix column indexes
    * @ax: double/complex array of length ap[n], matrix values
    * @threads: # of threads to be created for analysis, factor, refactor, solve, and sort
    * @rperm: pre-allocated buffer for extracting row permutation vector (length n)
    * @cperm: pre-allocated buffer for extracting column permutation vector (length n)
    * @rscale: pre-allocated buffer for extracting row scaling vector (length n)
    * @cscale: pre-allocated buffer for extracting column scaling vector (length n)
    */
    virtual int _CDECL_ Analyze2
    (
        _IN_ bool is_complex,
        _IN_ int n,
        _IN_ const int ap[],
        _IN_ const int ai[],
        _IN_ const double ax[], /*can be NULL if unavailable when analysis*/
        _IN_ int threads, /*0=use all physical cores. -1=use all logical cores*/
        _OUT_ int rperm[], /*can be NULL if not needed*/
        _OUT_ int cperm[], /*can be NULL if not needed*/
        _OUT_ double rscale[], /*can be NULL if not needed*/
        _OUT_ double cscale[] /*can be NULL if not needed*/
    ) = 0;

    /*
    * ExtractFactors: extracts LU factors (L is with diagonal and U is without diagonal) (call this routine after matrix has been factorized with pivoting).
    * @lp: pre-allocated buffer for extracting L's row pointers (length n+1)
    * @li: pre-allocated buffer for extracting L's column indexes (length oparm[5])
    * @lx: pre-allocated buffer for extracting L's values (length oparm[5] for double or oparm[5]*2 for complex)
    * @up: pre-allocated buffer for extracting U's row pointers (length n+1)
    * @ui: pre-allocated buffer for extracting U's column indexes (length oparm[6])
    * @ux: pre-allocated buffer for extracting U's values (length oparm[6] for double or oparm[6]*2 for complex)
    * @rperm: pre-allocated buffer for extracting row permutation vector (length n)
    * @cperm: pre-allocated buffer for extracting final column permutation (including pivoting) vector (length n)
    * @rscale: pre-allocated buffer for extracting row scaling vector (length n)
    * @cscale: pre-allocated buffer for extracting column scaling vector (length n)
    */
    virtual int _CDECL_ ExtractFactors
    (
        _OUT_ size_t lp[],
        _OUT_ int li[],
        _OUT_ double lx[], /*can be NULL if not needed*/
        _OUT_ size_t up[],
        _OUT_ int ui[],
        _OUT_ double ux[], /*can be NULL if not needed*/
        _OUT_ int rperm[], /*can be NULL if not needed*/
        _OUT_ int cperm[], /*can be NULL if not needed*/
        _OUT_ double rscale[], /*can be NULL if not needed*/
        _OUT_ double cscale[] /*can be NULL if not needed*/
    ) = 0;
};

struct __cktso_l_dummy
{
    /*
    * DestroySolver: frees everything and destroys solver instance.
    */
    virtual int _CDECL_ DestroySolver
    (
    ) = 0;

    /*
    * Analyze: analyzes matrix for static pivoting and fillin-reduction ordering. Previous matrix is first destroyed and then new matrix is created.
    * @is_complex: complex or real
    * @n: matrix dimension
    * @ap: integer array of length n+1, matrix row pointers
    * @ai: integer array of length ap[n], matrix column indexes
    * @ax: double/complex array of length ap[n], matrix values
    * @threads: # of threads to be created for analysis, factor, refactor, solve, and sort
    */
    virtual int _CDECL_ Analyze
    (
        _IN_ bool is_complex, 
        _IN_ long long n,
        _IN_ const long long ap[],
        _IN_ const long long ai[],
        _IN_ const double ax[], /*can be NULL if unavailable when analysis*/
        _IN_ int threads /*0=use all physical cores. -1=use all logical cores*/
    ) = 0;

    /*
    * Factorize: factorizes matrix with pivoting (call this routine after matrix has been analyzed).
    * @ax: double/complex array of length ap[n], matrix values
    * @fast: whether to use fast factorization (skips symbolic, but checks pivots)
    */
    virtual int _CDECL_ Factorize
    (
        _IN_ const double ax[], 
        _IN_ bool fast
    ) = 0;

    /*
    * Refactorize: refactorizes matrix without pivoting (call this routine after matrix has been factorized with pivoting).
    * @ax: double/complex array of length ap[n], matrix values
    */
    virtual int _CDECL_ Refactorize
    (
        _IN_ const double ax[]
    ) = 0;

    /*
    * Solve: solves Ax=b after A is factorized (call this routine after matrix has been factorized or refactorized).
    * @b: double/complex array of length n to specify right-hand-side vector
    * @x: double/complex array of length n to get solution
    * @force_seq: force to use sequential solve (if not, solver automatically decides sequential or parallel)
    * @row0_column1: row or column mode
    */
    virtual int _CDECL_ Solve
    (
        _IN_ const double b[],
        _OUT_ double x[], /*x address can be same as b address for overwriting x on b*/
        _IN_ bool force_seq, 
        _IN_ bool row0_column1
    ) = 0;

    /*
    * SolveMV: solves Ax=b with multiple b after A is factorized (call this routine after matrix has been factorized or refactorized).
    * @nrhs: number of right-hand-side vectors
    * @b: double/complex array of length ld_b*nrhs to specify right-hand-side vectors, vector by vector
    * @ld_b: leading dimension of two-dimensional matrix b
    * @x: double/complex array of length ld_x*nrhs to get solutions, vector by vector
    * @ld_x: leading dimension of two-dimensional matrix x
    * @row0_column1: row or column mode
    */
    virtual int _CDECL_ SolveMV
    (
        _IN_ size_t nrhs,
        _IN_ const double b[],
        _IN_ size_t ld_b, /*if ld_b=0, ld_b=n*/
        _OUT_ double x[], /*x address can be same as b address for overwriting x on b*/
        _IN_ size_t ld_x, /*if ld_x=0, ld_x=n*/
        _IN_ bool row0_column1
    ) = 0;

    /*
    * SortFactors: sorts indexes and/or values of LU factors (call this routine after matrix has been factorized with pivoting).
    * @also_sort_values: whether to sort values as well, false for only sorting indexes
    */
    virtual int _CDECL_ SortFactors
    (
        _IN_ bool also_sort_values /*whether to sort values as well*/
    ) = 0;

    /*
    * Statistics: calculates # of floating-point operations and memory access volumes (call this routine after matrix has been factorized with pivoting).
    * @factor_flops: pointer to a 64b integer to retrieve flops of factor
    * @solve_flops: pointer to a 64b integer to retrieve flops of solve
    * @factor_mem: pointer to a 64b integer to retrieve memory access volume of factor in bytes
    * @solver_mem: pointer to a 64b integer to retrieve memory access volume of solve in bytes
    * @row0_column1: row or column mode (affects solve_mem)
    * @scaling: whether scaling is counted in flops & memory access
    * @fuse_mac: whether multiply-and-accumulate is counted as a single fused operation
    */
    virtual int _CDECL_ Statistics
    (
        _OUT_ long long *factor_flops, /*can be NULL if not needed*/
        _OUT_ long long *solve_flops, /*can be NULL if not needed*/
        _OUT_ long long *factor_mem, /*can be NULL if not needed*/
        _OUT_ long long *solve_mem, /*can be NULL if not needed*/
        _IN_ bool row0_column1,
        _IN_ char scaling, /*0 for disabled, >0 for enabled, <0 for using solver setting*/
        _IN_ bool fuse_mac
    ) = 0;

    /*
    * CleanUpGarbage: cleans up redundant memory.
    */
    virtual int _CDECL_ CleanUpGarbage
    (
    ) = 0;

    /*
    * Determinant: calculates determinant (mantissa*10^exponent, where 1 <= abs(mantissa) < 10) (call this routine after matrix has been factorized or refactorized).
    * @mantissa: mantissa of determinant (double or complex)
    * @exponent: exponent of determinant
    */
    virtual int _CDECL_ Determinant
    (
        _OUT_ double *mantissa,
        _OUT_ double *exponent
    ) = 0;

    /*
    * FactorizeAndSolve: factorizes matrix with pivoting and then solves Ax=b (call this routine after matrix has been analyzed).
    * @ax: double/complex array of length ap[n], matrix values
    * @b: double/complex array of length n to specify right-hand-side vector
    * @x: double/complex array of length n to get solution
    * @row0_column1: row or column mode
    */
    virtual int _CDECL_ FactorizeAndSolve
    (
        _IN_ const double ax[],
        _IN_ const double b[],
        _OUT_ double x[], /*x address can be same as b address for overwriting x on b*/
        _IN_ bool row0_column1
    ) = 0;

    /*
    * RefactorizeAndSolve: refactorizes matrix without pivoting and then solves Ax=b (call this routine after matrix has been factorized with pivoting).
    * @ax: double/complex array of length ap[n], matrix values
    * @b: double/complex array of length n to specify right-hand-side vector
    * @x: double/complex array of length n to get solution
    * @row0_column1: row or column mode
    */
    virtual int _CDECL_ RefactorizeAndSolve
    (
        _IN_ const double ax[],
        _IN_ const double b[],
        _OUT_ double x[], /*x address can be same as b address for overwriting x on b*/
        _IN_ bool row0_column1
    ) = 0;

    /*
    * Analyze2: analyzes matrix for static pivoting and fillin-reduction ordering. Previous matrix is first destroyed and then new matrix is created.
    * @is_complex: complex or real
    * @n: matrix dimension
    * @ap: integer array of length n+1, matrix row pointers
    * @ai: integer array of length ap[n], matrix column indexes
    * @ax: double/complex array of length ap[n], matrix values
    * @threads: # of threads to be created for analysis, factor, refactor, solve, and sort
    * @rperm: pre-allocated buffer for extracting row permutation vector (length n)
    * @cperm: pre-allocated buffer for extracting column permutation vector (length n)
    * @rscale: pre-allocated buffer for extracting row scaling vector (length n)
    * @cscale: pre-allocated buffer for extracting column scaling vector (length n)
    */
    virtual int _CDECL_ Analyze2
    (
        _IN_ bool is_complex,
        _IN_ long long n,
        _IN_ const long long ap[],
        _IN_ const long long ai[],
        _IN_ const double ax[], /*can be NULL if unavailable when analysis*/
        _IN_ int threads, /*0=use all physical cores. -1=use all logical cores*/
        _OUT_ long long rperm[], /*can be NULL if not needed*/
        _OUT_ long long cperm[], /*can be NULL if not needed*/
        _OUT_ double rscale[], /*can be NULL if not needed*/
        _OUT_ double cscale[] /*can be NULL if not needed*/
    ) = 0;

    /*
    * ExtractFactors: extracts LU factors (L is with diagonal and U is without diagonal) (call this routine after matrix has been factorized with pivoting).
    * @lp: pre-allocated buffer for extracting L's row pointers (length n+1)
    * @li: pre-allocated buffer for extracting L's column indexes (length oparm[5])
    * @lx: pre-allocated buffer for extracting L's values (length oparm[5] for double or oparm[5]*2 for complex)
    * @up: pre-allocated buffer for extracting U's row pointers (length n+1)
    * @ui: pre-allocated buffer for extracting U's column indexes (length oparm[6])
    * @ux: pre-allocated buffer for extracting U's values (length oparm[6] for double or oparm[6]*2 for complex)
    * @rperm: pre-allocated buffer for extracting row permutation vector (length n)
    * @cperm: pre-allocated buffer for extracting final column permutation (including pivoting) vector (length n)
    * @rscale: pre-allocated buffer for extracting row scaling vector (length n)
    * @cscale: pre-allocated buffer for extracting column scaling vector (length n)
    */
    virtual int _CDECL_ ExtractFactors
    (
        _OUT_ size_t lp[],
        _OUT_ long long li[],
        _OUT_ double lx[], /*can be NULL if not needed*/
        _OUT_ size_t up[],
        _OUT_ long long ui[],
        _OUT_ double ux[], /*can be NULL if not needed*/
        _OUT_ long long rperm[], /*can be NULL if not needed*/
        _OUT_ long long cperm[], /*can be NULL if not needed*/
        _OUT_ double rscale[], /*can be NULL if not needed*/
        _OUT_ double cscale[] /*can be NULL if not needed*/
    ) = 0;
};
#endif/*__cplusplus*/

/*
* Raw C interface
*/
#ifdef __cplusplus
extern "C" {
#endif

/*
* CKTSO_CreateSolver (CKTSO_L_CreateSolver): creates solve instance and retrieves parameter array pointers.
* @inst: pointer to an ICktSo (ICktSo_L) instance that retrieves created instance handle
* @iparm: pointer to input parameter list array (see annotations above)
* @oparm: pointer to output parameter list array (see annotations above)
*/
int CKTSO_CreateSolver
(
    _OUT_ ICktSo *inst, 
    _OUT_ int **iparm, /*can be NULL if not needed*/
    _OUT_ const long long **oparm /*can be NULL if not needed*/
);
int CKTSO_L_CreateSolver
(
    _OUT_ ICktSo_L *inst,
    _OUT_ int **iparm, /*can be NULL if not needed*/
    _OUT_ const long long **oparm /*can be NULL if not needed*/
);
int CKTSO_CreateSolverNoCheck /*no check on whether AVX2 and FMA are supported*/
(
    _OUT_ ICktSo *inst,
    _OUT_ int **iparm, /*can be NULL if not needed*/
    _OUT_ const long long **oparm /*can be NULL if not needed*/
);
int CKTSO_L_CreateSolverNoCheck /*no check on whether AVX2 and FMA are supported*/
(
    _OUT_ ICktSo_L *inst,
    _OUT_ int **iparm, /*can be NULL if not needed*/
    _OUT_ const long long **oparm /*can be NULL if not needed*/
);

/*
* CKTSO_DestroySolver (CKTSO_L_DestroySolver): frees everything and destroys solver.
* @inst: solver instance handle returned by CKTSO_CreateSolver (CKTSO_L_CreateSolver)
*/
int CKTSO_DestroySolver
(
    _IN_ ICktSo inst
);
int CKTSO_L_DestroySolver
(
    _IN_ ICktSo_L inst
);

/*
* CKTSO_Analyze (CKTSO_L_Analyze): analyzes matrix for static pivoting and fillin-reduction ordering. Previous matrix is first destroyed and then new matrix is created.
* @inst: solver instance handle returned by CKTSO_CreateSolver (CKTSO_L_CreateSolver)
* @is_complex: complex or real
* @n: matrix dimension
* @ap: integer array of length n+1, matrix row pointers
* @ai: integer array of length ap[n], matrix column indexes
* @ax: double/complex array of length ap[n], matrix values
* @threads: # of threads to be created for analysis, factor, refactor, solve, and sort
*/
int CKTSO_Analyze
(
    _IN_ ICktSo inst, 
    _IN_ bool is_complex, 
    _IN_ int n, 
    _IN_ const int ap[], 
    _IN_ const int ai[], 
    _IN_ const double ax[], /*can be NULL if unavailable when analysis*/
    _IN_ int threads /*0=use all physical cores. -1=use all logical cores*/
);
int CKTSO_L_Analyze
(
    _IN_ ICktSo_L inst,
    _IN_ bool is_complex, 
    _IN_ long long n, 
    _IN_ const long long ap[], 
    _IN_ const long long ai[], 
    _IN_ const double ax[], /*can be NULL if unavailable when analysis*/
    _IN_ int threads /*0=use all physical cores. -1=use all logical cores*/
);

/*
* CKTSO_Factorize (CKTSO_L_Factorize): factorizes matrix with pivoting (call this routine after matrix has been analyzed)
* @inst: solver instance handle returned by CKTSO_CreateSolver (CKTSO_L_CreateSolver)
* @ax: double/complex array of length ap[n], matrix values
* @fast: whether to use fast factorization (skips symbolic, but checks pivots)
*/
int CKTSO_Factorize
(
    _IN_ ICktSo inst, 
    _IN_ const double ax[], 
    _IN_ bool fast
);
int CKTSO_L_Factorize
(
    _IN_ ICktSo_L inst,
    _IN_ const double ax[], 
    _IN_ bool fast
);

/*
* CKTSO_Refactorize (CKTSO_L_Refactorize): refactorizes matrix without pivoting (call this routine after matrix has been factorized with pivoting).
* @inst: solver instance handle returned by CKTSO_CreateSolver (CKTSO_L_CreateSolver)
* @ax: double/complex array of length ap[n], matrix values
*/
int CKTSO_Refactorize
(
    _IN_ ICktSo inst, 
    _IN_ const double ax[]
);
int CKTSO_L_Refactorize
(
    _IN_ ICktSo_L inst,
    _IN_ const double ax[]
);

/*
* CKTSO_Solve (CKTSO_L_Solve): solves Ax=b after A is factorized (call this routine after matrix has been factorized or refactorized).
* @inst: solver instance handle returned by CKTSO_CreateSolver (CKTSO_L_CreateSolver)
* @b: double/complex array of length n to specify right-hand-side vector
* @x: double/complex array of length n to get solution
* @force_seq: force to use sequential solve (if not, solver automatically decides sequential or parallel)
* @row0_column1: row or column mode
*/
int CKTSO_Solve
(
    _IN_ ICktSo inst, 
    _IN_ const double b[], 
    _OUT_ double x[], /*x address can be same as b address for overwriting x on b*/
    _IN_ bool force_seq, 
    _IN_ bool row0_column1
);
int CKTSO_L_Solve
(
    _IN_ ICktSo_L inst,
    _IN_ const double b[], 
    _OUT_ double x[], /*x address can be same as b address for overwriting x on b*/
    _IN_ bool force_seq, 
    _IN_ bool row0_column1
);

/*
* CKTSO_SolveMV (CKTSO_L_SolveMV): solves Ax=b with multiple b after A is factorized (call this routine after matrix has been factorized or refactorized).
* @inst: solver instance handle returned by CKTSO_CreateSolver (CKTSO_L_CreateSolver)
* @nrhs: number of right-hand-side vectors
* @b: double/complex array of length ld_b*nrhs to specify right-hand-side vectors, vector by vector
* @ld_b: leading dimension of two-dimensional matrix b
* @x: double/complex array of length ld_x*nrhs to get solutions, vector by vector
* @ld_x: leading dimension of two-dimensional matrix x
* @row0_column1: row or column mode
*/
int CKTSO_SolveMV
(
    _IN_ ICktSo inst,
    _IN_ size_t nrhs, 
    _IN_ const double b[],
    _IN_ size_t ld_b, /*if ld_b=0, ld_b=n*/
    _OUT_ double x[], /*x address can be same as b address for overwriting x on b*/
    _IN_ size_t ld_x, /*if ld_x=0, ld_x=n*/
    _IN_ bool row0_column1
);
int CKTSO_L_SolveMV
(
    _IN_ ICktSo_L inst,
    _IN_ size_t nrhs,
    _IN_ const double b[],
    _IN_ size_t ld_b, /*if ld_b=0, ld_b=n*/
    _OUT_ double x[], /*x address can be same as b address for overwriting x on b*/
    _IN_ size_t ld_x, /*if ld_x=0, ld_x=n*/
    _IN_ bool row0_column1
);

/*
* CKTSO_SortFactors (CKTSO_L_SortFactors): sorts indexes and/or values of LU factors (call this routine after matrix has been factorized with pivoting).
* @inst: solver instance handle returned by CKTSO_CreateSolver (CKTSO_L_CreateSolver)
* @also_sort_values: whether to sort values as well, false for only sorting indexes
*/
int CKTSO_SortFactors
(
    _IN_ ICktSo inst,
    _IN_ bool also_sort_values /*whether to sort values as well*/
);
int CKTSO_L_SortFactors
(
    _IN_ ICktSo_L inst,
    _IN_ bool also_sort_values /*whether to sort values as well*/
);

/*
* CKTSO_Statistics (CKTSO_L_Statistics): calculates # of floating-point operations and memory access volumes (call this routine after matrix has been factorized with pivoting).
* @inst: solver instance handle returned by CKTSO_CreateSolver (CKTSO_L_CreateSolver)
* @factor_flops: pointer to a 64b integer to retrieve flops of factor
* @solve_flops: pointer to a 64b integer to retrieve flops of solve
* @factor_mem: pointer to a 64b integer to retrieve memory access volume of factor in bytes
* @solver_mem: pointer to a 64b integer to retrieve memory access volume of solve in bytes
* @row0_column1: row or column mode (affects solve_mem)
* @scaling: whether scaling is counted in flops & memory access
* @fuse_mac: whether multiply-and-accumulate is counted as a single fused operation
*/
int CKTSO_Statistics
(
    _IN_ ICktSo inst, 
    _OUT_ long long *factor_flops, /*can be NULL if not needed*/
    _OUT_ long long *solve_flops, /*can be NULL if not needed*/
    _OUT_ long long *factor_mem, /*can be NULL if not needed*/
    _OUT_ long long *solve_mem, /*can be NULL if not needed*/
    _IN_ bool row0_column1,
    _IN_ char scaling, /*0 for disabled, >0 for enabled, <0 for using solver setting*/
    _IN_ bool fuse_mac
);
int CKTSO_L_Statistics
(
    _IN_ ICktSo_L inst,
    _OUT_ long long *factor_flops, /*can be NULL if not needed*/
    _OUT_ long long *solve_flops, /*can be NULL if not needed*/
    _OUT_ long long *factor_mem, /*can be NULL if not needed*/
    _OUT_ long long *solve_mem, /*can be NULL if not needed*/
    _IN_ bool row0_column1,
    _IN_ char scaling, /*0 for disabled, >0 for enabled, <0 for using solver setting*/
    _IN_ bool fuse_mac
);

/*
* CKTSO_CleanUpGarbage (CKTSO_L_CleanUpGarbage): cleans up redundant memory
* @inst: solver instance handle returned by CKTSO_CreateSolver (CKTSO_L_CreateSolver)
*/
int CKTSO_CleanUpGarbage
(
    _IN_ ICktSo inst
);
int CKTSO_L_CleanUpGarbage
(
    _IN_ ICktSo_L inst
);

/*
* CKTSO_Determinant (CKTSO_L_Determinant): calculates determinant (mantissa*10^exponent, where 1 <= abs(mantissa) < 10) (call this routine after matrix has been factorized or refactorized).
* @inst: solver instance handle returned by CKTSO_CreateSolver (CKTSO_L_CreateSolver)
* @mantissa: mantissa of determinant (double or complex)
* @exponent: exponent of determinant
*/
int CKTSO_Determinant
(
    _IN_ ICktSo inst, 
    _OUT_ double *mantissa, 
    _OUT_ double *exponent
);
int CKTSO_L_Determinant
(
    _IN_ ICktSo_L inst,
    _OUT_ double *mantissa,
    _OUT_ double *exponent
);

/*
* CKTSO_FactorizeAndSolve (CKTSO_L_FactorizeAndSolve): factorizes matrix with pivoting and then solves Ax=b (call this routine after matrix has been analyzed).
* @inst: solver instance handle returned by CKTSO_CreateSolver (CKTSO_L_CreateSolver)
* @ax: double/complex array of length ap[n], matrix values
* @b: double/complex array of length n to specify right-hand-side vector
* @x: double/complex array of length n to get solution
* @row0_column1: row or column mode
*/
int CKTSO_FactorizeAndSolve
(
    _IN_ ICktSo inst,
    _IN_ const double ax[],
    _IN_ const double b[],
    _OUT_ double x[], /*x address can be same as b address for overwriting x on b*/
    _IN_ bool row0_column1
);
int CKTSO_L_FactorizeAndSolve
(
    _IN_ ICktSo_L inst,
    _IN_ const double ax[],
    _IN_ const double b[],
    _OUT_ double x[], /*x address can be same as b address for overwriting x on b*/
    _IN_ bool row0_column1
);

/*
* CKTSO_RefactorizeAndSolve (CKTSO_L_RefactorizeAndSolve): refactorizes matrix without pivoting and then solves Ax=b (call this routine after matrix has been factorized with pivoting).
* @inst: solver instance handle returned by CKTSO_CreateSolver (CKTSO_L_CreateSolver)
* @ax: double/complex array of length ap[n], matrix values
* @b: double/complex array of length n to specify right-hand-side vector
* @x: double/complex array of length n to get solution
* @row0_column1: row or column mode
*/
int CKTSO_RefactorizeAndSolve
(
    _IN_ ICktSo inst,
    _IN_ const double ax[],
    _IN_ const double b[],
    _OUT_ double x[], /*x address can be same as b address for overwriting x on b*/
    _IN_ bool row0_column1
);
int CKTSO_L_RefactorizeAndSolve
(
    _IN_ ICktSo_L inst,
    _IN_ const double ax[],
    _IN_ const double b[],
    _OUT_ double x[], /*x address can be same as b address for overwriting x on b*/
    _IN_ bool row0_column1
);

/*
* CKTSO_Analyze2 (CKTSO_L_Analyze2): analyzes matrix for static pivoting and fillin-reduction ordering. Previous matrix is first destroyed and then new matrix is created.
* @inst: solver instance handle returned by CKTSO_CreateSolver (CKTSO_L_CreateSolver)
* @is_complex: complex or real
* @n: matrix dimension
* @ap: integer array of length n+1, matrix row pointers
* @ai: integer array of length ap[n], matrix column indexes
* @ax: double/complex array of length ap[n], matrix values
* @threads: # of threads to be created for analysis, factor, refactor, solve, and sort
* @rperm: pre-allocated buffer for extracting row permutation vector (length n)
* @cperm: pre-allocated buffer for extracting column permutation vector (length n)
* @rscale: pre-allocated buffer for extracting row scaling vector (length n)
* @cscale: pre-allocated buffer for extracting column scaling vector (length n)
*/
int CKTSO_Analyze2
(
    _IN_ ICktSo inst,
    _IN_ bool is_complex,
    _IN_ int n,
    _IN_ const int ap[],
    _IN_ const int ai[],
    _IN_ const double ax[], /*can be NULL if unavailable when analysis*/
    _IN_ int threads, /*0=use all physical cores. -1=use all logical cores*/
    _OUT_ int rperm[], /*can be NULL if not needed*/
    _OUT_ int cperm[], /*can be NULL if not needed*/
    _OUT_ double rscale[], /*can be NULL if not needed*/
    _OUT_ double cscale[] /*can be NULL if not needed*/
);
int CKTSO_L_Analyze2
(
    _IN_ ICktSo_L inst,
    _IN_ bool is_complex,
    _IN_ long long n,
    _IN_ const long long ap[],
    _IN_ const long long ai[],
    _IN_ const double ax[], /*can be NULL if unavailable when analysis*/
    _IN_ int threads, /*0=use all physical cores. -1=use all logical cores*/
    _OUT_ long long rperm[], /*can be NULL if not needed*/
    _OUT_ long long cperm[], /*can be NULL if not needed*/
    _OUT_ double rscale[], /*can be NULL if not needed*/
    _OUT_ double cscale[] /*can be NULL if not needed*/
);

/*
* CKTSO_ExtractFactors (CKTSO_L_ExtractFactors): extracts LU factors (L is with diagonal and U is without diagonal) (call this routine after matrix has been factorized with pivoting).
* @inst: solver instance handle returned by CKTSO_CreateSolver (CKTSO_L_CreateSolver)
* @lp: pre-allocated buffer for extracting L's row pointers (length n+1)
* @li: pre-allocated buffer for extracting L's column indexes (length oparm[5])
* @lx: pre-allocated buffer for extracting L's values (length oparm[5] for double or oparm[5]*2 for complex)
* @up: pre-allocated buffer for extracting U's row pointers (length n+1)
* @ui: pre-allocated buffer for extracting U's column indexes (length oparm[6])
* @ux: pre-allocated buffer for extracting U's values (length oparm[6] for double or oparm[6]*2 for complex)
* @rperm: pre-allocated buffer for extracting row permutation vector (length n)
* @cperm: pre-allocated buffer for extracting final column permutation (including pivoting) vector (length n)
* @rscale: pre-allocated buffer for extracting row scaling vector (length n)
* @cscale: pre-allocated buffer for extracting column scaling vector (length n)
*/
int CKTSO_ExtractFactors
(
    _IN_ ICktSo inst,
    _OUT_ size_t lp[],
    _OUT_ int li[],
    _OUT_ double lx[], /*can be NULL if not needed*/
    _OUT_ size_t up[],
    _OUT_ int ui[],
    _OUT_ double ux[], /*can be NULL if not needed*/
    _OUT_ int rperm[], /*can be NULL if not needed*/
    _OUT_ int cperm[], /*can be NULL if not needed*/
    _OUT_ double rscale[], /*can be NULL if not needed*/
    _OUT_ double cscale[] /*can be NULL if not needed*/
);
int CKTSO_L_ExtractFactors
(
    _IN_ ICktSo_L inst,
    _OUT_ size_t lp[],
    _OUT_ long long li[],
    _OUT_ double lx[], /*can be NULL if not needed*/
    _OUT_ size_t up[],
    _OUT_ long long ui[],
    _OUT_ double ux[], /*can be NULL if not needed*/
    _OUT_ long long rperm[], /*can be NULL if not needed*/
    _OUT_ long long cperm[], /*can be NULL if not needed*/
    _OUT_ double rscale[], /*can be NULL if not needed*/
    _OUT_ double cscale[] /*can be NULL if not needed*/
);

#ifdef __cplusplus
}
#endif

#endif
