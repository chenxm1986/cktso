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
} cktsow_t;

_PUBLIC_ cktsow_t *CKTSOW_CreateSolver()
{
	cktsow_t *wrapper = (cktsow_t *)malloc(sizeof(cktsow_t));
	if (NULL == wrapper) return NULL;
	int ret = CKTSO_CreateSolver(&wrapper->handle, &wrapper->iparm, &wrapper->oparm);
	if (ret != 0)
	{
		free(wrapper);
		return NULL;
	}
	return wrapper;
}

_PUBLIC_ cktsow_t *CKTSOW_CreateSolverNoCheck()
{
	cktsow_t *wrapper = (cktsow_t *)malloc(sizeof(cktsow_t));
	if (NULL == wrapper) return NULL;
	int ret = CKTSO_CreateSolverNoCheck(&wrapper->handle, &wrapper->iparm, &wrapper->oparm);
	if (ret != 0)
	{
		free(wrapper);
		return NULL;
	}
	return wrapper;
}

_PUBLIC_ void CKTSOW_SetInputParameter(cktsow_t *wrapper, int id, int value)
{
	wrapper->iparm[id] = value;
}

_PUBLIC_ long long CKTSOW_GetOutputParameter(cktsow_t *wrapper, int id)
{
	return wrapper->oparm[id];
}

_PUBLIC_ int CKTSOW_DestroySolver(cktsow_t *wrapper)
{
	int ret = CKTSO_DestroySolver(wrapper->handle);
	free(wrapper);
	return ret;
}

_PUBLIC_ int CKTSOW_Analyze(cktsow_t *wrapper, bool is_complex, int n, const int ap[], const int ai[], const double ax[], int threads)
{
	return CKTSO_Analyze(wrapper->handle, is_complex, n, ap, ai, ax, threads);
}

_PUBLIC_ int CKTSOW_Factorize(cktsow_t *wrapper, const double ax[], bool fast)
{
	return CKTSO_Factorize(wrapper->handle, ax, fast);
}

_PUBLIC_ int CKTSOW_Refactorize(cktsow_t *wrapper, const double ax[])
{
	return CKTSO_Refactorize(wrapper->handle, ax);
}

_PUBLIC_ int CKTSOW_Solve(cktsow_t *wrapper, const double b[], double x[], bool force_seq, bool row0_column1)
{
	return CKTSO_Solve(wrapper->handle, b, x, force_seq, row0_column1);
}

_PUBLIC_ int CKTSOW_SolveMV(cktsow_t *wrapper, size_t nrhs, const double b[], size_t ld_b, double x[], size_t ld_x, bool row0_column1)
{
	return CKTSO_SolveMV(wrapper->handle, nrhs, b, ld_b, x, ld_x, row0_column1);
}

_PUBLIC_ int CKTSOW_SortFactors(cktsow_t *wrapper, bool sort_values)
{
	return CKTSO_SortFactors(wrapper->handle, sort_values);
}

_PUBLIC_ int CKTSOW_Statistics(cktsow_t *wrapper, long long *factor_flops, long long *solve_flops, long long *factor_mem, long long *solve_mem, bool row0_column1, char scaling, bool fuse_mac)
{
	return CKTSO_Statistics(wrapper->handle, factor_flops, solve_flops, factor_mem, solve_mem, row0_column1, scaling, fuse_mac);
}

_PUBLIC_ int CKTSOW_CleanUpGarbage(cktsow_t *wrapper)
{
	return CKTSO_CleanUpGarbage(wrapper->handle);
}

_PUBLIC_ int CKTSOW_Determinant(cktsow_t *wrapper, double *mantissa, double *exponent)
{
	return CKTSO_Determinant(wrapper->handle, mantissa, exponent);
}

_PUBLIC_ int CKTSOW_FactorizeAndSolve(cktsow_t *wrapper, const double ax[], const double b[], double x[], bool row0_column1)
{
	return CKTSO_FactorizeAndSolve(wrapper->handle, ax, b, x, row0_column1);
}

_PUBLIC_ int CKTSOW_RefactorizeAndSolve(cktsow_t *wrapper, const double ax[], const double b[], double x[], bool row0_column1)
{
	return CKTSO_RefactorizeAndSolve(wrapper->handle, ax, b, x, row0_column1);
}

_PUBLIC_ int CKTSOW_Analyze2(cktsow_t *wrapper, bool is_complex, int n, const int ap[], const int ai[], const double ax[], int threads, int rperm[], int cperm[], double rscale[], double cscale[])
{
	return CKTSO_Analyze2(wrapper->handle, is_complex, n, ap, ai, ax, threads, rperm, cperm, rscale, cscale);
}

_PUBLIC_ int CKTSOW_ExtractFactors(cktsow_t *wrapper, size_t lp[], int li[], double lx[], size_t up[], int ui[], double ux[], int rperm[], int cperm[], double rscale[], double cscale[])
{
	return CKTSO_ExtractFactors(wrapper->handle, lp, li, lx, up, ui, ux, rperm, cperm, rscale, cscale);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
	ICktSo_L handle;
	int *iparm;
	const long long *oparm;
} cktsow_l_t;

_PUBLIC_ cktsow_l_t *CKTSOW_L_CreateSolver()
{
	cktsow_l_t *wrapper = (cktsow_l_t *)malloc(sizeof(cktsow_l_t));
	if (NULL == wrapper) return NULL;
	int ret = CKTSO_L_CreateSolver(&wrapper->handle, &wrapper->iparm, &wrapper->oparm);
	if (ret != 0)
	{
		free(wrapper);
		return NULL;
	}
	return wrapper;
}

_PUBLIC_ cktsow_l_t *CKTSOW_L_CreateSolverNoCheck()
{
	cktsow_l_t *wrapper = (cktsow_l_t *)malloc(sizeof(cktsow_l_t));
	if (NULL == wrapper) return NULL;
	int ret = CKTSO_L_CreateSolverNoCheck(&wrapper->handle, &wrapper->iparm, &wrapper->oparm);
	if (ret != 0)
	{
		free(wrapper);
		return NULL;
	}
	return wrapper;
}

_PUBLIC_ void CKTSOW_L_SetInputParameter(cktsow_l_t *wrapper, int id, int value)
{
	wrapper->iparm[id] = value;
}

_PUBLIC_ long long CKTSOW_L_GetOutputParameter(cktsow_l_t *wrapper, int id)
{
	return wrapper->oparm[id];
}

_PUBLIC_ int CKTSOW_L_DestroySolver(cktsow_l_t *wrapper)
{
	int ret = CKTSO_L_DestroySolver(wrapper->handle);
	free(wrapper);
	return ret;
}

_PUBLIC_ int CKTSOW_L_Analyze(cktsow_l_t *wrapper, bool is_complex, long long n, const long long ap[], const long long ai[], const double ax[], int threads)
{
	return CKTSO_L_Analyze(wrapper->handle, is_complex, n, ap, ai, ax, threads);
}

_PUBLIC_ int CKTSOW_L_Factorize(cktsow_l_t *wrapper, const double ax[], bool fast)
{
	return CKTSO_L_Factorize(wrapper->handle, ax, fast);
}

_PUBLIC_ int CKTSOW_L_Refactorize(cktsow_l_t *wrapper, const double ax[])
{
	return CKTSO_L_Refactorize(wrapper->handle, ax);
}

_PUBLIC_ int CKTSOW_L_Solve(cktsow_l_t *wrapper, const double b[], double x[], bool force_seq, bool row0_column1)
{
	return CKTSO_L_Solve(wrapper->handle, b, x, force_seq, row0_column1);
}

_PUBLIC_ int CKTSOW_L_SolveMV(cktsow_l_t *wrapper, size_t nrhs, const double b[], size_t ld_b, double x[], size_t ld_x, bool row0_column1)
{
	return CKTSO_L_SolveMV(wrapper->handle, nrhs, b, ld_b, x, ld_x, row0_column1);
}

_PUBLIC_ int CKTSOW_L_SortFactors(cktsow_l_t *wrapper, bool sort_values)
{
	return CKTSO_L_SortFactors(wrapper->handle, sort_values);
}

_PUBLIC_ int CKTSOW_L_Statistics(cktsow_l_t *wrapper, long long *factor_flops, long long *solve_flops, long long *factor_mem, long long *solve_mem, bool row0_column1, char scaling, bool fuse_mac)
{
	return CKTSO_L_Statistics(wrapper->handle, factor_flops, solve_flops, factor_mem, solve_mem, row0_column1, scaling, fuse_mac);
}

_PUBLIC_ int CKTSOW_L_CleanUpGarbage(cktsow_l_t *wrapper)
{
	return CKTSO_L_CleanUpGarbage(wrapper->handle);
}

_PUBLIC_ int CKTSOW_L_Determinant(cktsow_l_t *wrapper, double *mantissa, double *exponent)
{
	return CKTSO_L_Determinant(wrapper->handle, mantissa, exponent);
}

_PUBLIC_ int CKTSOW_L_FactorizeAndSolve(cktsow_l_t *wrapper, const double ax[], const double b[], double x[], bool row0_column1)
{
	return CKTSO_L_FactorizeAndSolve(wrapper->handle, ax, b, x, row0_column1);
}

_PUBLIC_ int CKTSOW_L_RefactorizeAndSolve(cktsow_l_t *wrapper, const double ax[], const double b[], double x[], bool row0_column1)
{
	return CKTSO_L_RefactorizeAndSolve(wrapper->handle, ax, b, x, row0_column1);
}

_PUBLIC_ int CKTSOW_L_Analyze2(cktsow_l_t *wrapper, bool is_complex, long long n, const long long ap[], const long long ai[], const double ax[], int threads, long long rperm[], long long cperm[], double rscale[], double cscale[])
{
	return CKTSO_L_Analyze2(wrapper->handle, is_complex, n, ap, ai, ax, threads, rperm, cperm, rscale, cscale);
}

_PUBLIC_ int CKTSOW_L_ExtractFactors(cktsow_l_t *wrapper, size_t lp[], long long li[], double lx[], size_t up[], long long ui[], double ux[], long long rperm[], long long cperm[], double rscale[], double cscale[])
{
	return CKTSO_L_ExtractFactors(wrapper->handle, lp, li, lx, up, ui, ux, rperm, cperm, rscale, cscale);
}
