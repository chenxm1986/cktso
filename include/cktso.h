/*
* CKTSO is a high-performance parallel linear system solver for SPICE-based circuit simulations.
*/

/*
* version 202207
* build 20220715
*/

#ifndef __CKTSO__
#define __CKTSO__

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
* -99:  license error
* -100: unknown error
********************************/

/********** input parameters int [] **********
* input parm[0]:  timer. [default 0]: no timer | >0: microsecond/us-level timer | <0: millisecond/ms-level timer
* input parm[1]:  pivoting tolerance (in millionth). [default 1000 (=0.001)]
* input parm[2]:  ordering method. [default 0]: select best from 8 methods | 1~8: corresponding single method | two digits in decimalism (20<=x0<=80) means selecting best from first x methods | <0: no ordering
* input parm[3]:  threshold (percentage) for dense node detection in ordering. [default 1000 (=10.0)]
* input parm[4]:  metric for ordering method selection. [default >=0]: use flops | <0: use nnz
* input parm[5]:  max supernode size. [default -1]: no limitation
* input parm[6]:  minimum # of columns for supernode detection. [default 64]
* input parm[7]:  scaling. [default 0]: no scaling
* input parm[8]:  whether right-hand-vector is very sparse (e.g., # of nonzeros < n/10), only effective for sequential column-mode solve. [default 0]
* input parm[9]:  automatic threads control. [default 1]
* input parm[10]: memory growth ratio (percentage). [default 150 (=1.5)]
* input parm[11]: initial # of rows for supernode creation. [default 16]
* input parm[12]: static pivoting method. 0: conventional | [default >0]: fill-in aware | <0: column size aware
* input parm[13]: sync method. [default >=0]: blocked wait | <0: busy wait
********************************/

/********** output parameters long long [] **********
* output parm[0]:  time (in microsecond/us) of CKTSO(_L)_Analyze
* output parm[1]:  time (in microsecond/us) of CKTSO(_L)_Factorize or CKTSO(_L)_Refactorize
* output parm[2]:  time (in microsecond/us) of CKTSO(_L)_Solve
* output parm[3]:  time (in microsecond/us) of CKTSO(_L)_SortFactors
* output parm[4]:  # of off-diagonal pivots
* output parm[5]:  nnz(L), including diagonal
* output parm[6]:  nnz(U), excluding diagonal
* output parm[7]:  # of supernodes
* output parm[8]:  selected ordering method
* output parm[9]:  singular row index when -6 is returned
* output parm[10]: # of realloc invoked during factor
* output parm[11]: memory requirement (in bytes) when -4 is returned (for the last malloc/realloc failure)
* output parm[12]: current memory usage (in bytes)
* output parm[13]: maximum memory usage (in bytes)
* output parm[14]: # of rows completed with pivoting reuse in CKTSO(_L)_Factorize
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
#if (defined(_MSC_VER) && !defined(_M_X64) && !defined(_WIN64))
#define _CDECL_ __cdecl
#else
#define _CDECL_
#endif

struct __cktso_dummy
{
    /*
    * DestroySolver: frees everything and destroys solver instance
    */
    virtual int _CDECL_ DestroySolver
    (
    ) = 0;

    /*
    * Analyze: analyzes matrix for static pivoting and fillin-reduction ordering
    * @n: matrix dimension
    * @ap: integer array of length n+1, matrix row pointers
    * @ai: integer array of length ap[n], matrix column indexes
    * @ax: double array of length ap[n], matrix values
    * @row0_column1: 0 means row mode and 1 means column mode
    * @threads: # of threads
    */
    virtual int _CDECL_ Analyze
    (
        _IN_ int n,
        _IN_ const int ap[],
        _IN_ const int ai[],
        _IN_ const double ax[],
        _IN_ bool row0_column1, 
        _IN_ int threads /*0=use all physical cores. -1=use all logical cores*/
    ) = 0;

    /*
    * Factorize: factorizes matrix with partial pivoting
    * Call this routine after Analyze has been called
    * @ax: double array of length ap[n], matrix values
    * @fast: whether to use fast factorization
    */
    virtual int _CDECL_ Factorize
    (
        _IN_ const double ax[], 
        _IN_ bool fast
    ) = 0;

    /*
    * Refactorize: refactorizes matrix without partial pivoting
    * Call this routine after Factorize has been called
    * @ax: double array of length ap[n], matrix values
    */
    virtual int _CDECL_ Refactorize
    (
        _IN_ const double ax[]
    ) = 0;

    /*
    * Solve: solves Ax=b when A is factorized
    * Call this routine after Factorize or Refactorize has been called
    * @b: double array of length n to specify right-hand-side vector
    * @x: double array of length n to get solution
    * @force_seq: force to use sequential solve (if not, solver automatically decides sequential or parallel)
    */
    virtual int _CDECL_ Solve
    (
        _IN_ const double b[],
        _OUT_ double x[], /*x address can be same as b address*/
        _IN_ bool force_seq
    ) = 0;

    /*
    * SortFactors: sorts indexes and/or values of LU factors
    * Call this routine after Factorize has been called
    * @also_sort_values: whether to sort values as well
    */
    virtual int _CDECL_ SortFactors
    (
        _IN_ bool also_sort_values /*whether to sort values as well*/
    ) = 0;

    /*
    * Statistics: calculates # of floating-point operations and memory access volumes
    * Call this routine after Factorize has been called
    * @factor_flops: pointer to a 64b integer to retrieve flops of factor
    * @solve_flops: pointer to a 64b integer to retrieve flops of solve
    * @factor_mem: pointer to a 64b integer to retrieve memory access volume of factor in bytes
    * @solver_mem: pointer to a 64b integer to retrieve memory access volume of solve in bytes
    */
    virtual int _CDECL_ Statistics
    (
        _OUT_ long long *factor_flops, 
        _OUT_ long long *solve_flops, 
        _OUT_ long long *factor_mem, 
        _OUT_ long long *solve_mem
    ) = 0;

    /*
    * CleanupGarbage: cleans up redundant memory
    */
    virtual int _CDECL_ CleanupGarbage
    (
    ) = 0;
};

struct __cktso_l_dummy
{
    /*
    * DestroySolver: frees everything and destroys solver instance
    */
    virtual int _CDECL_ DestroySolver
    (
    ) = 0;

    /*
    * Analyze: analyzes matrix for static pivoting and fillin-reduction ordering
    * @n: matrix dimension
    * @ap: integer array of length n+1, matrix row pointers
    * @ai: integer array of length ap[n], matrix column indexes
    * @ax: double array of length ap[n], matrix values
    * @row0_column1: 0 means row mode and 1 means column mode
    * @threads: # of threads
    */
    virtual int _CDECL_ Analyze
    (
        _IN_ long long n,
        _IN_ const long long ap[],
        _IN_ const long long ai[],
        _IN_ const double ax[],
        _IN_ bool row0_column1, 
        _IN_ int threads /*0=use all physical cores. -1=use all logical cores*/
    ) = 0;

    /*
    * Factorize: factorizes matrix with partial pivoting
    * Call this routine after Analyze has been called
    * @ax: double array of length ap[n], matrix values
    * @fast: whether to use fast factorization
    */
    virtual int _CDECL_ Factorize
    (
        _IN_ const double ax[], 
        _IN_ bool fast
    ) = 0;

    /*
    * Refactorize: refactorizes matrix without partial pivoting
    * Call this routine after Factorize has been called
    * @ax: double array of length ap[n], matrix values
    */
    virtual int _CDECL_ Refactorize
    (
        _IN_ const double ax[]
    ) = 0;

    /*
    * Solve: solves Ax=b when A is factorized
    * Call this routine after Factorize or Refactorize has been called
    * @b: double array of length n to specify right-hand-side vector
    * @x: double array of length n to get solution
    * @force_seq: force to use sequential solve (if not, solver automatically decides sequential or parallel)
    */
    virtual int _CDECL_ Solve
    (
        _IN_ const double b[],
        _OUT_ double x[], /*x address can be same as b address*/
        _IN_ bool force_seq
    ) = 0;

    /*
    * SortFactors: sorts indexes and/or values of LU factors
    * Call this routine after Factorize has been called
    * @also_sort_values: whether to sort values as well
    */
    virtual int _CDECL_ SortFactors
    (
        _IN_ bool also_sort_values /*whether to sort values as well*/
    ) = 0;

    /*
    * Statistics: calculates # of floating-point operations and memory access volumes
    * Call this routine after Factorize has been called
    * @factor_flops: pointer to a 64b integer to retrieve flops of factor
    * @solve_flops: pointer to a 64b integer to retrieve flops of solve
    * @factor_mem: pointer to a 64b integer to retrieve memory access volume of factor in bytes
    * @solver_mem: pointer to a 64b integer to retrieve memory access volume of solve in bytes
    */
    virtual int _CDECL_ Statistics
    (
        _OUT_ long long *factor_flops, 
        _OUT_ long long *solve_flops, 
        _OUT_ long long *factor_mem, 
        _OUT_ long long *solve_mem
    ) = 0;

    /*
    * CleanupGarbage: cleans up redundant memory
    */
    virtual int _CDECL_ CleanupGarbage
    (
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
* CKTSO_CreateSolver (CKTSO_L_CreateSolver): creates solve instance and retrieves parameter array pointers
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

/*
* CKTSO_DestroySolver (CKTSO_L_DestroySolver): frees everything and destroys solver
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
* CKTSO_Analyze (CKTSO_L_Analyze): analyzes matrix for static pivoting and fillin-reduction ordering
* @inst: solver instance handle returned by CKTSO_CreateSolver (CKTSO_L_CreateSolver)
* @n: matrix dimension
* @ap: integer array of length n+1, matrix row pointers
* @ai: integer array of length ap[n], matrix column indexes
* @ax: double array of length ap[n], matrix values
* @row0_column1: 0 means row mode and 1 means column mode
* @threads: # of threads
*/
int CKTSO_Analyze
(
    _IN_ ICktSo inst, 
    _IN_ int n, 
    _IN_ const int ap[], 
    _IN_ const int ai[], 
    _IN_ const double ax[], 
    _IN_ bool row0_column1, 
    _IN_ int threads /*0=use all physical cores. -1=use all logical cores*/
);

int CKTSO_L_Analyze
(
    _IN_ ICktSo_L inst,
    _IN_ long long n, 
    _IN_ const long long ap[], 
    _IN_ const long long ai[], 
    _IN_ const double ax[], 
    _IN_ bool row0_column1, 
    _IN_ int threads /*0=use all physical cores. -1=use all logical cores*/
);

/*
* CKTSO_Factorize (CKTSO_L_Factorize): factorizes matrix with partial pivoting
* Call this routine after CKTSO_Analyze (CKTSO_L_Analyze) has been called
* @inst: solver instance handle returned by CKTSO_CreateSolver (CKTSO_L_CreateSolver)
* @ax: double array of length ap[n], matrix values
* @fast: whether to use fast factorization
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
* CKTSO_Refactorize (CKTSO_L_Refactorize): refactorizes matrix without partial pivoting
* Call this routine after CKTSO_Factorize (CKTSO_L_Factorize) has been called
* @inst: solver instance handle returned by CKTSO_CreateSolver (CKTSO_L_CreateSolver)
* @ax: double array of length ap[n], matrix values
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
* CKTSO_Solve (CKTSO_L_Solve): solves Ax=b when A is factorized
* Call this routine after CKTSO_Factorize (CKTSO_L_Factorize) or CKTSO_Refactorize (CKTSO_L_Refactorize) has been called
* @inst: solver instance handle returned by CKTSO_CreateSolver (CKTSO_L_CreateSolver)
* @b: double array of length n to specify right-hand-side vector
* @x: double array of length n to get solution
* @force_seq: force to use sequential solve (if not, solver automatically decides sequential or parallel)
*/
int CKTSO_Solve
(
    _IN_ ICktSo inst, 
    _IN_ const double b[], 
    _OUT_ double x[], /*x address can be same as b address*/
    _IN_ bool force_seq
);

int CKTSO_L_Solve
(
    _IN_ ICktSo_L inst,
    _IN_ const double b[], 
    _OUT_ double x[], /*x address can be same as b address*/
    _IN_ bool force_seq
);

/*
* CKTSO_SortFactors (CKTSO_L_SortFactors): sorts indexes and/or values of LU factors
* Call this routine after CKTSO_Factorize (CKTSO_L_Factorize) has been called
* @also_sort_values: whether to sort values as well
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
* CKTSO_Statistics (CKTSO_L_Statistics): calculates # of floating-point operations and memory access volumes
* Call this routine after CKTSO_Factorize (CKTSO_L_Factorize) has been called
* @inst: solver instance handle returned by CKTSO_CreateSolver (CKTSO_L_CreateSolver)
* @factor_flops: pointer to a 64b integer to retrieve flops of factor
* @solve_flops: pointer to a 64b integer to retrieve flops of solve
* @factor_mem: pointer to a 64b integer to retrieve memory access volume of factor in bytes
* @solver_mem: pointer to a 64b integer to retrieve memory access volume of solve in bytes
*/
int CKTSO_Statistics
(
    _IN_ ICktSo inst, 
    _OUT_ long long *factor_flops, 
    _OUT_ long long *solve_flops, 
    _OUT_ long long *factor_mem, 
    _OUT_ long long *solve_mem
);

int CKTSO_L_Statistics
(
    _IN_ ICktSo_L inst,
    _OUT_ long long *factor_flops, 
    _OUT_ long long *solve_flops, 
    _OUT_ long long *factor_mem, 
    _OUT_ long long *solve_mem
);

/*
* CKTSO_CleanupGarbage (CKTSO_L_CleanupGarbage): cleans up redundant memory
* @inst: solver instance handle returned by CKTSO_CreateSolver (CKTSO_L_CreateSolver)
*/
int CKTSO_CleanupGarbage
(
    _IN_ ICktSo inst
);

int CKTSO_L_CleanupGarbage
(
    _IN_ ICktSo_L inst
);

#ifdef __cplusplus
}
#endif

#endif
