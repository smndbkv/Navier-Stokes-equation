#ifndef MSR_MATRIX_H
#define MSR_MATRIX_H
#include <stdio.h>
#include <math.h>
#include "pthread.h"

#define EPS_64 (1e-64)
#define EPS (1.12e-16)
#define LEN (1234)
#define CMP(x, y) \
    ((fabs(((x) - (y))) <= fabs(((0.5) * (EPS) * ((x) + (y))))) ? 1 : 0)

void thread_rows(int n, int p, int k, int &i1, int &i2);
void msr_matrix_mult_vector(double *A, int *I, int n, double *x, double *b, int p, int k);
void init_msr_matrix(double *A, int *I, int n);
double set(double *A, int *I, int n, int i, int j, double value);
double get(double *A, int *I, int n, int i, int j);
void print_msr_matrix(double *A, int *I, int n, int maxprint = 10);

void print_array(double *a, int n, int maxprint = 10);
void print_array(int *a, int n, int maxprint = 10);

void reduce_sum(int p, double *a, int n);
double scalar_product(double *x, double *y, int n, int p, int k, double *sp);
void mult_sub_vector(double *x, double *y, int n, double t, int p, int k);
void apply_preconditioner_msr_matrix(double *M, int *I, int n, double *v, double *r, int p, int k);
int minimal_residual_msr_matrix(double *A, int *I, int n, double *M, double *b, double *x, double *u, double *v, double *r, double eps, int maxit, int p, int k, double *sp);
int minimal_residual_msr_matrix(double *A, int *I, int n, double *b, double *x, double *u, double *v, double *r, double eps, int maxit, int maxsteps, int p, int k, double *sp);

// double get_r2(double *x, double *x_exact, int n);

#endif