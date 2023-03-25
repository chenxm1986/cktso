#include "cktso.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#ifdef _WIN32
#define _PUBLIC_ __declspec(dllexport)
#pragma comment(lib, "cktso.lib")
#pragma comment(lib, "cktso_l.lib")
#else
#define _PUBLIC_ __attribute__((visibility("default")))
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
	ICktSo handle;
	int *iparm;
	const long long *oparm;
} __cktso_wrapper;

_PUBLIC_ __cktso_wrapper *CKTSO2_CreateSolver()
{
	__cktso_wrapper *wrapper = (__cktso_wrapper *)malloc(sizeof(__cktso_wrapper));
	if (NULL == wrapper) return NULL;
	int ret = CKTSO_CreateSolver(&wrapper->handle, &wrapper->iparm, &wrapper->oparm);
	if (ret != 0)
	{
		free(wrapper);
		return NULL;
	}
	return wrapper;
}

_PUBLIC_ __cktso_wrapper *CKTSO2_CreateSolverNoCheck()
{
	__cktso_wrapper *wrapper = (__cktso_wrapper *)malloc(sizeof(__cktso_wrapper));
	if (NULL == wrapper) return NULL;
	int ret = CKTSO_CreateSolverNoCheck(&wrapper->handle, &wrapper->iparm, &wrapper->oparm);
	if (ret != 0)
	{
		free(wrapper);
		return NULL;
	}
	return wrapper;
}

_PUBLIC_ void CKTSO2_SetInputParameter(__cktso_wrapper *wrapper, int id, int value)
{
	wrapper->iparm[id] = value;
}

_PUBLIC_ long long CKTSO2_GetOutputParameter(__cktso_wrapper *wrapper, int id)
{
	return wrapper->oparm[id];
}

_PUBLIC_ int CKTSO2_DestroySolver(__cktso_wrapper *wrapper)
{
	int ret = CKTSO_DestroySolver(wrapper->handle);
	free(wrapper);
	return ret;
}

_PUBLIC_ int CKTSO2_Analyze(__cktso_wrapper *wrapper, bool is_complex, int n, const int ap[], const int ai[], const double ax[], int threads)
{
	return CKTSO_Analyze(wrapper->handle, is_complex, n, ap, ai, ax, threads);
}

_PUBLIC_ int CKTSO2_Factorize(__cktso_wrapper *wrapper, const double ax[], bool fast)
{
	return CKTSO_Factorize(wrapper->handle, ax, fast);
}

_PUBLIC_ int CKTSO2_Refactorize(__cktso_wrapper *wrapper, const double ax[])
{
	return CKTSO_Refactorize(wrapper->handle, ax);
}

_PUBLIC_ int CKTSO2_Solve(__cktso_wrapper *wrapper, const double b[], double x[], bool force_seq, bool row0_column1)
{
	return CKTSO_Solve(wrapper->handle, b, x, force_seq, row0_column1);
}

_PUBLIC_ int CKTSO2_SolveMV(__cktso_wrapper *wrapper, size_t nrhs, const double b[], size_t ld_b, double x[], size_t ld_x, bool row0_column1)
{
	return CKTSO_SolveMV(wrapper->handle, nrhs, b, ld_b, x, ld_x, row0_column1);
}

_PUBLIC_ int CKTSO2_SortFactors(__cktso_wrapper *wrapper, bool sort_values)
{
	return CKTSO_SortFactors(wrapper->handle, sort_values);
}

_PUBLIC_ int CKTSO2_Statistics(__cktso_wrapper *wrapper, long long *factor_flops, long long *solve_flops, long long *factor_mem, long long *solve_mem, bool row0_column1)
{
	return CKTSO_Statistics(wrapper->handle, factor_flops, solve_flops, factor_mem, solve_mem, row0_column1);
}

_PUBLIC_ int CKTSO2_CleanUpGarbage(__cktso_wrapper *wrapper)
{
	return CKTSO_CleanUpGarbage(wrapper->handle);
}

_PUBLIC_ int CKTSO2_Determinant(__cktso_wrapper *wrapper, double *mantissa, double *exponent)
{
	return CKTSO_Determinant(wrapper->handle, mantissa, exponent);
}

_PUBLIC_ int CKTSO2_FactorizeAndSolve(__cktso_wrapper *wrapper, const double ax[], const double b[], double x[], bool row0_column1)
{
	return CKTSO_FactorizeAndSolve(wrapper->handle, ax, b, x, row0_column1);
}

_PUBLIC_ int CKTSO2_RefactorizeAndSolve(__cktso_wrapper *wrapper, const double ax[], const double b[], double x[], bool row0_column1)
{
	return CKTSO_RefactorizeAndSolve(wrapper->handle, ax, b, x, row0_column1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
	ICktSo_L handle;
	int *iparm;
	const long long *oparm;
} __cktso_l_wrapper;

_PUBLIC_ __cktso_l_wrapper *CKTSO2_L_CreateSolver()
{
	__cktso_l_wrapper *wrapper = (__cktso_l_wrapper *)malloc(sizeof(__cktso_l_wrapper));
	if (NULL == wrapper) return NULL;
	int ret = CKTSO_L_CreateSolver(&wrapper->handle, &wrapper->iparm, &wrapper->oparm);
	if (ret != 0)
	{
		free(wrapper);
		return NULL;
	}
	return wrapper;
}

_PUBLIC_ __cktso_l_wrapper *CKTSO2_L_CreateSolverNoCheck()
{
	__cktso_l_wrapper *wrapper = (__cktso_l_wrapper *)malloc(sizeof(__cktso_l_wrapper));
	if (NULL == wrapper) return NULL;
	int ret = CKTSO_L_CreateSolverNoCheck(&wrapper->handle, &wrapper->iparm, &wrapper->oparm);
	if (ret != 0)
	{
		free(wrapper);
		return NULL;
	}
	return wrapper;
}

_PUBLIC_ void CKTSO2_L_SetInputParameter(__cktso_l_wrapper *wrapper, int id, int value)
{
	wrapper->iparm[id] = value;
}

_PUBLIC_ long long CKTSO2_L_GetOutputParameter(__cktso_l_wrapper *wrapper, int id)
{
	return wrapper->oparm[id];
}

_PUBLIC_ int CKTSO2_L_DestroySolver(__cktso_l_wrapper *wrapper)
{
	int ret = CKTSO_L_DestroySolver(wrapper->handle);
	free(wrapper);
	return ret;
}

_PUBLIC_ int CKTSO2_L_Analyze(__cktso_l_wrapper *wrapper, bool is_complex, long long n, const long long ap[], const long long ai[], const double ax[], int threads)
{
	return CKTSO_L_Analyze(wrapper->handle, is_complex, n, ap, ai, ax, threads);
}

_PUBLIC_ int CKTSO2_L_Factorize(__cktso_l_wrapper *wrapper, const double ax[], bool fast)
{
	return CKTSO_L_Factorize(wrapper->handle, ax, fast);
}

_PUBLIC_ int CKTSO2_L_Refactorize(__cktso_l_wrapper *wrapper, const double ax[])
{
	return CKTSO_L_Refactorize(wrapper->handle, ax);
}

_PUBLIC_ int CKTSO2_L_Solve(__cktso_l_wrapper *wrapper, const double b[], double x[], bool force_seq, bool row0_column1)
{
	return CKTSO_L_Solve(wrapper->handle, b, x, force_seq, row0_column1);
}

_PUBLIC_ int CKTSO2_L_SolveMV(__cktso_l_wrapper *wrapper, size_t nrhs, const double b[], size_t ld_b, double x[], size_t ld_x, bool row0_column1)
{
	return CKTSO_L_SolveMV(wrapper->handle, nrhs, b, ld_b, x, ld_x, row0_column1);
}

_PUBLIC_ int CKTSO2_L_SortFactors(__cktso_l_wrapper *wrapper, bool sort_values)
{
	return CKTSO_L_SortFactors(wrapper->handle, sort_values);
}

_PUBLIC_ int CKTSO2_L_Statistics(__cktso_l_wrapper *wrapper, long long *factor_flops, long long *solve_flops, long long *factor_mem, long long *solve_mem, bool row0_column1)
{
	return CKTSO_L_Statistics(wrapper->handle, factor_flops, solve_flops, factor_mem, solve_mem, row0_column1);
}

_PUBLIC_ int CKTSO2_L_CleanUpGarbage(__cktso_l_wrapper *wrapper)
{
	return CKTSO_L_CleanUpGarbage(wrapper->handle);
}

_PUBLIC_ int CKTSO2_L_Determinant(__cktso_l_wrapper *wrapper, double *mantissa, double *exponent)
{
	return CKTSO_L_Determinant(wrapper->handle, mantissa, exponent);
}

_PUBLIC_ int CKTSO2_L_FactorizeAndSolve(__cktso_l_wrapper *wrapper, const double ax[], const double b[], double x[], bool row0_column1)
{
	return CKTSO_L_FactorizeAndSolve(wrapper->handle, ax, b, x, row0_column1);
}

_PUBLIC_ int CKTSO2_L_RefactorizeAndSolve(__cktso_l_wrapper *wrapper, const double ax[], const double b[], double x[], bool row0_column1)
{
	return CKTSO_L_RefactorizeAndSolve(wrapper->handle, ax, b, x, row0_column1);
}
