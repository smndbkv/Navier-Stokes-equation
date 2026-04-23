#include "msr_matrix.h"

void thread_rows(int n, int p, int k, int &i1, int &i2)
{
    i1 = n * k;
    i1 /= p;
    i2 = n * (k + 1);
    i2 /= p;
}

void msr_matrix_mult_vector(double *A, int *I, int n, double *x, double *b, int p, int k)
{
    int i1, i2, l, J;
    thread_rows(n, p, k, i1, i2);
    double s = 0;
    for (int i = i1; i < i2; i++)
    {
        s = A[i] * x[i];
        l = I[i + 1] - I[i];
        J = I[i];
        for (int j = 0; j < l; j++)
        {
            s += A[J + j] * x[I[J + j]];
        }
        b[i] = s;
    }
}

void init_msr_matrix(double *A, int *I, int n)
{
    int l, J;

    I[0] = n + 1;
    int ii = 2;
    for (int i = 1; i < n; i++)
    {
        I[i] = n + ii;
        ii += 2;
    }
    I[n] = n + ii - 1;

    J = I[0];
    I[J] = 1;
    for (int i = 1; i < n - 1; i++)
    {
        J = I[i];
        I[J] = i - 1;
        I[J + 1] = i + 1;
    }
    J = I[n - 1];
    I[J] = n - 2;

    for (int i = 0; i < n; i++)
    {
        A[i] = 0.2;
        l = I[i + 1] - I[i];
        J = I[i];
        for (int j = 0; j < l; j++)
        {
            A[J + j] = -0.1;
        }
    }
}

double set(double *A, int *I, int n, int i, int j, double value)
{
    (void)n;
    if (i == j)
    {
        return A[i];
    }
    int J1 = I[i], J2 = I[i + 1];

    for (int jj = J1; jj < J2; jj++)
    {
        if (I[jj] == j)
        {
            A[jj] = value;
        }
    }

    return 0;
}

double get(double *A, int *I, int n, int i, int j)
{
    (void)n;
    if (i == j)
    {
        return A[i];
    }
    int J1 = I[i], J2 = I[i + 1];

    for (int jj = J1; jj < J2; jj++)
    {
        if (I[jj] == j)
        {
            return A[jj];
        }
    }

    return 0;
}

void print_msr_matrix(double *A, int *I, int n, int maxprint)
{
    int nn = (n < maxprint ? n : maxprint);

    for (int i = 0; i < nn; i++)
    {
        for (int j = 0; j < nn; j++)
        {
            printf(" %10.3lf", get(A, I, n, i, j));
        }
        printf("\n");
    }
}

void print_array(double *a, int n, int maxprint)
{
    int nn = (n < maxprint ? n : maxprint);

    for (int i = 0; i < nn; i++)
    {
        printf(" %10.3lf", a[i]);
    }
    printf("\n");
}
void print_array(int *a, int n, int maxprint)
{
    int nn = (n < maxprint ? n : maxprint);
    for (int i = 0; i < nn; i++)
    {
        printf(" %10.d", a[i]);
    }
    printf("\n");
}

void reduce_sum(int p, double *a = nullptr, int n = 0)
{
    static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
    static pthread_cond_t c_in = PTHREAD_COND_INITIALIZER;
    static pthread_cond_t c_out = PTHREAD_COND_INITIALIZER;
    static int t_in = 0;
    static int t_out = 0;
    static double *r = nullptr;
    int i;

    if (p <= 1)
        return;

    pthread_mutex_lock(&m);

    if (r == nullptr)
    {
        r = a;
    }
    else
    {
        for (i = 0; i < n; i++)
        {
            r[i] += a[i];
        }
    }

    t_in++;
    if (t_in >= p)
    {
        t_out = 0;
        pthread_cond_broadcast(&c_in);
    }
    else
    {
        while (t_in < p)
        {
            pthread_cond_wait(&c_in, &m);
        }
    }

    if (a != r)
    {
        for (i = 0; i < n; i++)
        {
            a[i] = r[i];
        }
    }

    t_out++;
    if (t_out >= p)
    {
        t_in = 0;
        r = nullptr;
        pthread_cond_broadcast(&c_out);
    }
    else
    {
        while (t_out < p)
        {
            pthread_cond_wait(&c_out, &m);
        }
    }

    pthread_mutex_unlock(&m);
    return;
}

double scalar_product(double *x, double *y, int n, int p, int k, double *sp)
{
    int i, i1, i2;
    thread_rows(n, p, k, i1, i2);
    double s = 0;
    for (i = i1; i < i2; i++)
    {
        s += x[i] * y[i];
    }
    sp[k] = s;
    reduce_sum(p);
    s = 0;
    for (i = 0; i < p; i++)
    {
        s += sp[i];
    }
    return s;
}
void mult_sub_vector(double *x, double *y, int n, double t, int p, int k)
{
    int i, i1, i2;
    thread_rows(n, p, k, i1, i2);
    for (i = i1; i < i2; i++)
    {
        x[i] -= t * y[i];
    }
    reduce_sum(p);
}

void apply_preconditioner_msr_matrix(double *M, int *I, int n, double *v, double *r, int p, int k)
{
    (void)I;
    int i, i1, i2;
    thread_rows(n, p, k, i1, i2);
    for (i = i1; i < i2; i++)
    {
        v[i] = r[i] / M[i];
    }
    reduce_sum(p);
}

int minimal_residual_msr_matrix(double *A, int *I, int n, double *M, double *b, double *x, double *u, double *v, double *r, double eps, int maxit, int p, int k, double *sp)
{
    double c1, c2, prec, b_norm2, tau;
    int it;
    b_norm2 = scalar_product(b, b, n, p, k, sp);
    prec = eps * eps * b_norm2;
    msr_matrix_mult_vector(A, I, n, x, r, p, k);
    mult_sub_vector(r, b, n, 1, p, k);
    for (it = 0; it < maxit; it++)
    {
        apply_preconditioner_msr_matrix(M, I, n, v, r, p, k);
        msr_matrix_mult_vector(A, I, n, v, u, p, k);
        c1 = scalar_product(u, v, n, p, k, sp);
        c2 = scalar_product(u, u, n, p, k, sp);
        if (c1 < prec || c2 < prec)
        {
            break;
        }
        tau = c1 / c2;
        mult_sub_vector(x, v, n, tau, p, k);
        mult_sub_vector(r, u, n, tau, p, k);
    }
    if (it >= maxit)
    {
        return -1;
    }
    return it;
}
int minimal_residual_msr_matrix(double *A, int *I, int n, double *b, double *x, double *u, double *v, double *r, double eps, int maxit, int maxsteps, int p, int k, double *sp)
{
    int step, ret, its = 0;
    double *M = A;
    for (step = 0; step < maxsteps; step++)
    {
        ret = minimal_residual_msr_matrix(A, I, n, M, b, x, u, v, r, eps, maxit, p, k, sp);
        if (ret >= 0)
        {
            its += ret;
            break;
        }
        its += maxit;
    }
    if (step >= maxsteps)
    {
        return -1;
    }
    return its;
}

// double get_r2(double *x, double *x_exact, int n)
// {
//     double s1 = 0, s2 = 0;
//     for (int i = 0; i < n; i++)
//     {
//         s1 += fabs(x[i] - x_exact[i]);
//         s2 += fabs(x_exact[i]);
//     }
//     if (CMP(s2, 0))
//     {
//         return -1;
//     }
//     return s1 / s2;
// }