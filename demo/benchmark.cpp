#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS		1
#define _CRT_NONSTDC_NO_WARNINGS	1
#define _CRT_SECURE_NO_DEPRECATE	1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include "cktso.h"
#ifdef _MSC_VER
#pragma comment(lib, "cktso.lib")
#endif

bool ReadMtxFile(const char file[], int &n, int *&ap, int *&ai, double *&ax)
{
    FILE *fp = fopen(file, "r");
    if (NULL == fp)
    {
        printf("Cannot open file \"%s\".\n", file);
        return false;
    }

    char buf[256] = "\0";
    bool first = true;
    int pc = 0;
    int ptr = 0;
    while (fgets(buf, 256, fp) != NULL)
    {
        const char *p = buf;
        while (*p != '\0')
        {
            if (' ' == *p || '\t' == *p || '\r' == *p || '\n' == *p) ++p;
            else break;
        }

        if (*p == '\0') continue;
        else if (*p == '%') continue;
        else
        {
            if (first)
            {
                first = false;
                int r, c, nz;
                sscanf(p, "%d %d %d", &r, &c, &nz);
                if (r != c)
                {
                    printf("Matrix is not square because row = %d and column = %d.\n", r, c);
                    fclose(fp);
                    return false;
                }

                n = r;
                ap = new int [n + 1];
                ai = new int [nz];
                ax = new double [nz];
                if (NULL == ap || NULL == ai || NULL == ax)
                {
                    printf("Malloc for matrix failed.\n");
                    fclose(fp);
                    return false;
                }
                ap[0] = 0;
            }
            else
            {
                int r, c;
                double v;
                sscanf(p, "%d %d %lf", &r, &c, &v);
                --r;
                --c;
                ai[ptr] = r;
                ax[ptr] = v;
                if (c != pc)
                {
                    ap[c] = ptr;
                    pc = c;
                }
                ++ptr;
            }
        }
    }
    ap[n] = ptr;

    fclose(fp);
    return true;
}

double L2NormOfResidual(const int n, const int ap[], const int ai[], const double ax[], const double x[], const double b[])
{
    double s = 0.;
    for (int i = 0; i < n; ++i)
    {
        double r = 0.;
        const int start = ap[i];
        const int end = ap[i + 1];
        for (int p = start; p < end; ++p)
        {
            const int j = ai[p];
            r += ax[p] * x[j];
        }
        r -= b[i];
        s += r * r;
    }
    return sqrt(s);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: benchmark <mtx file> <# of threads>\n");
        printf("Example: benchmark add20.mtx 4\n");
        return -1;
    }

    int n;
    int *ap = NULL;
    int *ai = NULL;
    double *ax = NULL;
    if (!ReadMtxFile(argv[1], n, ap, ai, ax))
    {
        delete []ap;
        delete []ai;
        delete []ax;
        return -1;
    }

    double *b = new double [n + n];
    double *x = b + n;
    if (NULL == b)
    {
        printf("Malloc for b and x failed.\n");
        delete []ap;
        delete []ai;
        delete []ax;
        return -1;
    }
    for (int i = 0; i < n; ++i)
    {
        b[i] = (double)rand() / RAND_MAX * 100.;
        x[i] = 0.;
    }

    ICktSo instance = NULL;
    int *iparm;
    const long long *oparm;
    int ret = CKTSO_CreateSolver(&instance, &iparm, &oparm);
    if (ret < 0)
    {
        printf("Failed to create solver instance, return code = %d.\n", ret);
        delete []ap;
        delete []ai;
        delete []ax;
        delete []b;
        return ret;
    }
    iparm[0] = 1;

    instance->Analyze(n, ap, ai, ax, 0, atoi(argv[2]));
    printf("Analysis time = %lld us.\n", oparm[0]);

    long long min = LLONG_MAX;
    long long avg = 0;
    for (int i = 0; i < 100; ++i)
    {
        instance->Factorize(ax, true);
        if (oparm[1] < min) min = oparm[1];
        avg += oparm[1];
    }
    printf("Factorization average time = %lld us, min time = %lld us.\n", avg / 100, min);

    min = LLONG_MAX;
    avg = 0;
    for (int i = 0; i < 100; ++i)
    {
        instance->Refactorize(ax);
        if (oparm[1] < min) min = oparm[1];
        avg += oparm[1];
    }
    printf("Refactorization average time = %lld us, min time = %lld us.\n", avg / 100, min);

    min = LLONG_MAX;
    avg = 0;
    for (int i = 0; i < 100; ++i)
    {
        instance->Solve(b, x, false);
        if (oparm[2] < min) min = oparm[2];
        avg += oparm[2];
    }
    printf("Solve average time = %lld us, min time = %lld us.\n", avg / 100, min);

    printf("Residual = %g.\n", L2NormOfResidual(n, ap, ai, ax, x, b));

    printf("NNZ(L) = %lld, NNZ(U) = %lld.\n", oparm[5], oparm[6]);

    long long f1, f2;
    instance->Statistics(&f1, &f2, NULL, NULL);
    printf("Factorization flops = %lld, solve flops = %lld.\n", f1, f2);

    printf("Memory usage = %lld bytes, max memory usage = %lld bytes.\n", oparm[12], oparm[13]);

    delete []ap;
    delete []ai;
    delete []ax;
    delete []b;
    instance->DestroySolver();
    return 0;
}
