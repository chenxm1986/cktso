#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS		1
#define _CRT_NONSTDC_NO_WARNINGS	1
#define _CRT_SECURE_NO_DEPRECATE	1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cktso.h"
#ifdef _MSC_VER
#pragma comment(lib, "cktso.lib")
#endif

int main()
{
    int ret;
    int n = 6;
    int nnz = 13;
    double ax[13] = { 1.1, -7.7, 13.13, 2.2, 9.9, 8.8, -3.3, -4.4,
        11.11, 5.5, 10.1, 12.12, 6.6 };
    int ai[13] = { 0, 3, 4, 1, 4, 1, 2, 3, 2, 4, 0, 3, 5 };
    int ap[7] = { 0, 3, 5, 7, 8, 10, 13 };
    double b[6] = { 35.95, 53.9, 7.7, -17.6, 60.83, 98.18 };
    double x[6];
    int iter = 5;

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

    //Analyze matrix (one time)
    ret = instance->Analyze(n, ap, ai, ax, false, 0);
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

    //In circuit simulation, we need to solve the linear system many times with different values (but the symbolic pattern keeps unchanged)
    //The following code simulates such a case
    for (int j = 0; j < iter; ++j)
    {
        //Fill matrix and vector after model evaluation. Here we just randomly change the values to simulate the situation
        for (int i = 0; i < nnz; ++i) ax[i] *= (double)rand() / RAND_MAX * 2.;
        for (int i = 0; i < n; ++i) b[i] *= (double)rand() / RAND_MAX * 2.;

        //Factorize matrix
        if (0 == j) ret = instance->Factorize(ax, false);
        else ret = instance->Refactorize(ax);
        if (ret < 0)
        {
            printf("Failed to factorize matrix, return code = %d.\n", ret);
            instance->DestroySolver();
            return ret;
        }
        else
        {
            printf("Factorization [%d] time = %lld us.\n", j, oparm[1]);
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
            printf("Solve [%d] time = %lld us.\n", j, oparm[2]);
        }

        //Calculate residual (L2 norm). This is just for checking the result, not needed in circuit simulation
        double err = 0.;
        for (int r = 0; r < n; ++r)
        {
            double s = 0.;
            const int start = ap[r];
            const int end = ap[r + 1];
            for (int c = start; c < end; ++c)
            {
                s += ax[c] * x[ai[c]];
            }
            s -= b[r];
            err += s * s;
        }
        printf("Residual [%d] = %g.\n", j, sqrt(err));
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
