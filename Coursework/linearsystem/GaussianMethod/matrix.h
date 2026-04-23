#ifndef MATRIX_H
#define MATRIX_H
#include "io_status.h"
#include <iostream>
#include <math.h>

io_status init_matrix(double *a, int n, int s, double *b, double *x_exact,
                      const char *file_name);
void print_matrix(double *a, int n, int m, int r = MAX_PRINT);
void print(double *a, int n, int m = -1, int r = -1);
void get_block(double *a, int n, int m, int i, int j, double *c, int &v,
               int &h);
void set_block(double *a, int n, int m, int i, int j, double *c, int v,
               int h);
void get_right(double *b, int n, int m, int i, double *c, int &h);
void set_right(double *b, int m, int i, double *c, int h);
double f(int s, int n, int i, int j);
double dot_product(double *a, double *b, int n);
double get_r1(double *a, double *x, double *b, int n);
double get_r2(double *x, double *x_exact, int n);
void multy(double *a, double *b, int n, int m, int l, double *c);
void multy_right(double *g, double *b, int v, int h, double *d);
bool inverse(double *a, int n, double *c, double nrm_a);
bool main_element(double *a, int n, int m, int s, double *c, double *c_inv,
                  int &res_i, int &res_j, double nrm_a);
void swap(double *a, int n, int m, double *b, int s, int i, int j);
void add(double *a, double *b, int n, int m, double *c);
void sub(double *a, double *b, int n, int m, double *c);
bool is_zero(double *c, int v, int h, double nrm_a);
gauss_status gauss_method(int n, int m, double *a, double *b, double *x, double *c, double *g, double *d, double *f, int *p);

#endif