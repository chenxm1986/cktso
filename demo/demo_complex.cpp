#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS		1
#define _CRT_NONSTDC_NO_WARNINGS	1
#define _CRT_SECURE_NO_DEPRECATE	1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cktso.h"
#include "complex2real.hpp"
#ifdef _MSC_VER
#pragma comment(lib, "cktso.lib")
#endif

int main()
{
    int ret;
    int n = 6;
    double ax[26] = { 1., 1.1, -7., -7.7, 13., 13.13, 2., 2.2, 9., 9.9, 8., 8.8, -3., -3.3, -4., -4.4,
        11., 11.11, 5., 5.5, 10., 10.10, 12., 12.12, 6., 6.6 };
    int ai[13] = { 0, 3, 4, 1, 4, 1, 2, 3, 2, 4, 0, 3, 5 };
    int ap[7] = { 0, 3, 5, 7, 8, 10, 13 };
    double b[12] = { -2.9, 141.37, -20.8, 193.7, -6.4, 23.9, 7.2, -62.8, -21.66, 221.05, -36.36, 355.54 };
    double x[12];
    ComplexToReal c2r;

    //Create solver instance
    ICktSo instance = nullptr;
    int *iparm;
    const long long *oparm;
    ret = CKTSO_CreateSolver(&instance, &iparm, &oparm);
    if (ret < 0)
    {
        printf("Failed to create solver instance, return code = %d.\n", ret);
        return ret;
    }
    iparm[0] = 1; //enable high-precision timer

    //Analyze matrix
    c2r.ForAnalysis(n, ap, ai, ax);
    ret = instance->Analyze(c2r.n(), c2r.ap(), c2r.ai(), c2r.ax(), false, 0);
    if (ret < 0)
    {
        printf("Failed to analyze matrix, return code = %d.\n", ret);
        instance->DestroySolver();
        return ret;
    }
    else
    {
        printf("Analysis time = %lld us.\n", oparm[0]);
    }

    //Factorize matrix
    c2r.ForFactorization(ax);
    ret = instance->Factorize(c2r.ax(), false);
    if (ret < 0)
    {
        printf("Failed to factorize matrix, return code = %d.\n", ret);
        instance->DestroySolver();
        return ret;
    }
    else
    {
        printf("Factorization time = %lld us.\n", oparm[1]);
    }

    //Solve linear system
    ret = instance->Solve(b, x, false);
    if (ret < 0)
    {
        printf("Failed to solve linear system, return code = %d.\n", ret);
        instance->DestroySolver();
        return ret;
    }
    else
    {
        printf("Solve time = %lld us.\n", oparm[2]);
        for (int i = 0; i < n; ++i) printf("x[%d] = (%g, %g)\n", i, x[i + i], x[i + i + 1]);
    }

    //Print some statistical information
    printf("NNZ(L) = %lld.\n", oparm[5]);
    printf("NNZ(U) = %lld.\n", oparm[6]);
    printf("Memory usage = %lld bytes.\n", oparm[12]);
    printf("Max memory usage = %lld bytes.\n", oparm[13]);
    long long f1, f2;
    ret = instance->Statistics(&f1, &f2, NULL, NULL);
    if (ret < 0)
    {
        printf("Failed to retrieve statistics information, return code = %d.\n", ret);
        instance->DestroySolver();
        return ret;
    }
    else
    {
        printf("Factorization flops = %lld.\n", f1);
        printf("Solve flops = %lld.\n", f2);
    }

    instance->DestroySolver();
    return 0;
}
