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

typedef double complex[2];
#define cmul(z, a, b) \
{ \
    const double a0 = (a)[0]; \
    const double a1 = (a)[1]; \
    const double b0 = (b)[0]; \
    const double b1 = (b)[1]; \
    (z)[0] = a0 * b0 - a1 * b1; \
    (z)[1] = a0 * b1 + a1 * b0; \
}

double L2NormOfResidual(const int n, const int ap[], const int ai[], const complex ax[], const complex x[], const complex b[], bool row0_col1)
{
    if (row0_col1)
    {
        complex *bb = new complex[n];
        memcpy(bb, b, sizeof(complex) * n);
        for (int i = 0; i < n; ++i)
        {
            complex xx;
            xx[0] = x[i][0];
            xx[1] = x[i][1];
            const int start = ap[i];
            const int end = ap[i + 1];
            for (int p = start; p < end; ++p)
            {
                complex t;
                cmul(t, xx, ax[p]);
                bb[ai[p]][0] -= t[0];
                bb[ai[p]][1] -= t[1];
            }
        }
        double s = 0.;
        for (int i = 0; i < n; ++i)
        {
            s += bb[i][0] * bb[i][0] + bb[i][1] * bb[i][1];
        }
        delete[]bb;
        return sqrt(s);
    }
    else
    {
        double s = 0.;
        for (int i = 0; i < n; ++i)
        {
            complex r = { 0., 0. };
            const int start = ap[i];
            const int end = ap[i + 1];
            for (int p = start; p < end; ++p)
            {
                const int j = ai[p];
                complex t;
                cmul(t, ax[p], x[j]);
                r[0] += t[0];
                r[1] += t[1];
            }
            r[0] -= b[i][0];
            r[1] -= b[i][1];
            s += r[0] * r[0] + r[1] * r[1];
        }
        return sqrt(s);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: benchmark_complex <mtx file> <# of threads>\n");
        printf("Example: benchmark_complex add20.mtx 4\n");
        return -1;
    }

    int n;
    int *ap = NULL;
    int *ai = NULL;
    double *ax = NULL;
    double *cx = NULL;
    if (!ReadMtxFile(argv[1], n, ap, ai, ax))
    {
        delete []ap;
        delete []ai;
        delete []ax;
        return -1;
    }
    const int nnz = ap[n];
    cx = new double[nnz * 2];
    if (NULL == cx)
    {
        delete []ap;
        delete []ai;
        delete []ax;
        return -1;
    }
    for (int i = 0; i < nnz; ++i)
    {
        cx[i + i] = ax[i];
        cx[i + i + 1] = ax[i] * ((double)rand() / RAND_MAX - .5) * 2.;//randomly generate imaginary parts
    }
    delete []ax;

    double *b = new double[n * 4];
    double *x = b + n * 2;
    if (NULL == b)
    {
        printf("Malloc for b and x failed.\n");
        delete []ap;
        delete []ai;
        delete []cx;
        return -1;
    }
    for (int i = 0; i < n; ++i)
    {
        b[i + i] = (double)rand() / RAND_MAX * 100.;
        b[i + i + 1] = (double)rand() / RAND_MAX * 100.;
        x[i + i] = 0.;
        x[i + i + 1] = 0.;
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
        delete []b;
        delete []cx;
        return ret;
    }
    iparm[0] = 1;

    instance->Analyze(true, n, ap, ai, cx, atoi(argv[2]));
    printf("Analysis time = %lld us.\n", oparm[0]);

    long long min = LLONG_MAX;
    long long avg = 0;
    for (int i = 0; i < 100; ++i)
    {
        instance->Factorize(cx, true);
        if (oparm[1] < min) min = oparm[1];
        avg += oparm[1];
    }
    printf("Factorization average time = %lld us, min time = %lld us.\n", avg / 100, min);

    min = LLONG_MAX;
    avg = 0;
    for (int i = 0; i < 100; ++i)
    {
        instance->Refactorize(cx);
        if (oparm[1] < min) min = oparm[1];
        avg += oparm[1];
    }
    printf("Refactorization average time = %lld us, min time = %lld us.\n", avg / 100, min);

    min = LLONG_MAX;
    avg = 0;
    for (int i = 0; i < 100; ++i)
    {
        instance->Solve(b, x, false, false);
        if (oparm[2] < min) min = oparm[2];
        avg += oparm[2];
    }
    printf("Solve average time = %lld us, min time = %lld us.\n", avg / 100, min);

    printf("Residual = %g.\n", L2NormOfResidual(n, ap, ai, (complex *)cx, (complex *)x, (complex *)b, false));

    min = LLONG_MAX;
    avg = 0;
    for (int i = 0; i < 100; ++i)
    {
        instance->Solve(b, x, false, true);
        if (oparm[2] < min) min = oparm[2];
        avg += oparm[2];
    }
    printf("Transposed solve average time = %lld us, min time = %lld us.\n", avg / 100, min);

    printf("Residual = %g.\n", L2NormOfResidual(n, ap, ai, (complex *)cx, (complex *)x, (complex *)b, true));

    printf("NNZ(L) = %lld, NNZ(U) = %lld.\n", oparm[5], oparm[6]);

    long long f1, f2;
    instance->Statistics(&f1, &f2, NULL, NULL, false, -1, false);
    printf("Factorization flops = %lld, solve flops = %lld.\n", f1, f2);

    double mantissa[2], exponent;
    instance->Determinant(mantissa, &exponent);
    printf("Determinent = (%g,%g)*10^(%g).\n", mantissa[0], mantissa[1], exponent);

    printf("Memory usage = %lld bytes, max memory usage = %lld bytes.\n", oparm[12], oparm[13]);

    delete []ap;
    delete []ai;
    delete []b;
    delete []cx;
    instance->DestroySolver();
    return 0;
}
