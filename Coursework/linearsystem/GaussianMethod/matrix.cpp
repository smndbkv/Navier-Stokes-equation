#include "matrix.h"
#include "io_status.h"

io_status init_matrix(double *a, int n, int s, double *b, double *x_exact,
                      const char *file_name)
{
  if (s != 0)
  {
    for (int i = 0; i < n; i++)
    {
      for (int j = 0; j < n; j++)
      {
        a[i * n + j] = f(s, n, i + 1, j + 1);
      }
    }
  }
  else
  {
    FILE *fp;
    int i, j;
    if (!(fp = fopen(file_name, "r")))
      return io_status::ERROR_OPEN;

    for (i = 0; i < n; i++)
    {
      for (j = 0; j < n; j++)
      {
        if (fscanf(fp, "%lf", a + i * n + j) != 1)
        {
          fclose(fp);
          return ERROR_READ;
        }
      }
    }
    fclose(fp);
  }
  int p = (n - 1) / 2;
  for (int i = 0; i < n; i++)
  {
    b[i] = 0;
    x_exact[i] = (i + 1) % 2;
    for (int j = 0; j <= p; j++)
    {
      b[i] += a[i * n + 2 * j];
    }
  }
  return io_status::SUCCESS;
}

// inline __attribute__((always_inline)) void multy_slow(double *a, double *b, int n, int p, int m, double *c)
// {
//   int i, j, k;
//   int n4, m4;
//   double a0, a1, a2, a3;
//   double b0, b1, b2, b3;
//   double s00, s01, s02, s03;
//   double s10, s11, s12, s13;
//   double s20, s21, s22, s23;
//   double s30, s31, s32, s33;
//   double s;
//   double s0, s1, s2, s3;

//   n4 = n - n % 4;
//   m4 = m - m % 4;

//   for (i = 0; i < n4; i += 4)
//   {
//     for (j = 0; j < m4; j += 4)
//     {
//       s00 = s01 = s02 = s03 = 0.0;
//       s10 = s11 = s12 = s13 = 0.0;
//       s20 = s21 = s22 = s23 = 0.0;
//       s30 = s31 = s32 = s33 = 0.0;

//       for (k = 0; k < p; k++)
//       {
//         a0 = a[i * p + k];
//         a1 = a[(i + 1) * p + k];
//         a2 = a[(i + 2) * p + k];
//         a3 = a[(i + 3) * p + k];

//         b0 = b[k * m + j];
//         b1 = b[k * m + j + 1];
//         b2 = b[k * m + j + 2];
//         b3 = b[k * m + j + 3];

//         s00 += a0 * b0;
//         s01 += a0 * b1;
//         s02 += a0 * b2;
//         s03 += a0 * b3;

//         s10 += a1 * b0;
//         s11 += a1 * b1;
//         s12 += a1 * b2;
//         s13 += a1 * b3;

//         s20 += a2 * b0;
//         s21 += a2 * b1;
//         s22 += a2 * b2;
//         s23 += a2 * b3;

//         s30 += a3 * b0;
//         s31 += a3 * b1;
//         s32 += a3 * b2;
//         s33 += a3 * b3;
//       }

//       c[i * m + j] = s00;
//       c[i * m + j + 1] = s01;
//       c[i * m + j + 2] = s02;
//       c[i * m + j + 3] = s03;

//       c[(i + 1) * m + j] = s10;
//       c[(i + 1) * m + j + 1] = s11;
//       c[(i + 1) * m + j + 2] = s12;
//       c[(i + 1) * m + j + 3] = s13;

//       c[(i + 2) * m + j] = s20;
//       c[(i + 2) * m + j + 1] = s21;
//       c[(i + 2) * m + j + 2] = s22;
//       c[(i + 2) * m + j + 3] = s23;

//       c[(i + 3) * m + j] = s30;
//       c[(i + 3) * m + j + 1] = s31;
//       c[(i + 3) * m + j + 2] = s32;
//       c[(i + 3) * m + j + 3] = s33;
//     }

//     for (; j < m; j++)
//     {
//       s0 = 0;
//       s1 = 0;
//       s2 = 0;
//       s3 = 0;
//       for (k = 0; k < p; k++)
//       {
//         s0 += a[i * p + k] * b[k * m + j];
//         s1 += a[(i + 1) * p + k] * b[k * m + j];
//         s2 += a[(i + 2) * p + k] * b[k * m + j];
//         s3 += a[(i + 3) * p + k] * b[k * m + j];
//       }
//       c[i * m + j] = s0;
//       c[(i + 1) * m + j] = s1;
//       c[(i + 2) * m + j] = s2;
//       c[(i + 3) * m + j] = s3;
//     }
//   }

//   for (; i < n; i++)
//   {
//     for (j = 0; j < m; j++)
//     {
//       s = 0;
//       for (k = 0; k < p; k++)
//       {
//         s += a[i * p + k] * b[k * m + j];
//       }
//       c[i * m + j] = s;
//     }
//   }
// }

void get_block(double *a, int n, int m, int i, int j, double *c, int &v, int &h)
{
  int r = 0, t = 0;
  int k = n / m;
  int l = n - k * m;
  v = (i < k ? m : l);
  h = (j < k ? m : l);
  // double *pa = a + i * n * m + j * m;
  for (r = 0; r < v; r++)
  {
    for (t = 0; t < h; t++)
    {
      c[r * h + t] = a[(i * m + r) * n + j * m + t];
    }
  }
}

void set_block(double *a, int n, int m, int i, int j, double *c, int v, int h)
{
  for (int r = 0; r < v; r++)
  {
    for (int t = 0; t < h; t++)
    {
      a[(i * m + r) * n + (j * m + t)] = c[r * h + t];
    }
  }
}

void get_right(double *b, int n, int m, int i, double *c, int &h)
{
  h = (i < n / m ? m : n % m);
  for (int r = 0; r < h; r++)
  {
    c[r] = b[m * i + r];
  }
}
void set_right(double *b, int m, int i, double *c, int h)
{
  for (int r = 0; r < h; r++)
  {
    b[m * i + r] = c[r];
  }
}

void print_matrix(double *a, int n, int m, int r)
{
  int nn = std::min(n, r), mm = std::min(m, r);
  for (int i = 0; i < nn; i++)
  {
    for (int j = 0; j < mm; j++)
    {
      printf(" %10.3e", a[i * m + j]);
    }
    printf("\n");
  }
}
void print(double *a, int n, int m, int r)
{
  if (m == -1)
  {
    m = n;
  }
  if (r == -1)
  {
    r = n;
  }
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < m; j++)
    {
      printf(" %10.3e", a[i * m + j]);
    }
    printf("\n");
  }
}
double
f(int s, int n, int i, int j)
{
  switch (s)
  {
  case 1:
    return n - std::max(i, j) + 1;
    break;
  case 2:
    return std::max(i, j);
    break;
  case 3:
    return std::abs(i - j);
    break;
  case 4:
    return 1. / (i + j - 1);
    break;
  }
  return 0;
}

double
dot_product(double *a, double *b, int n)
{
  double s = 0;
  for (int i = 0; i < n; i++)
  {
    s += a[i] * b[i];
  }
  return s;
}
double
get_r1(double *a, double *x, double *b, int n)
{
  double s1 = 0, s2 = 0;
  for (int i = 0; i < n; i++)
  {
    s1 += fabs(dot_product(a + i * n, x, n) - b[i]);
    s2 += fabs(b[i]);
  }
  if (CMP(s2, 0))
  {
    return -1;
  }
  return s1 / s2;
}

double
get_r2(double *x, double *x_exact, int n)
{
  double s1 = 0, s2 = 0;
  for (int i = 0; i < n; i++)
  {
    s1 += fabs(x[i] - x_exact[i]);
    s2 += fabs(x_exact[i]);
  }
  if (CMP(s2, 0))
  {
    return -1;
  }
  return s1 / s2;
}

void multy(double *a, double *b, int n, int p, int m, double *c)
{
  int i, j, k;
  double s, s00, s01, s02, s10, s11, s12, s20, s21, s22, a0, a1, a2, b0, b1, b2;

  int n3 = n - n % 3;
  int m3 = m - m % 3;

  for (i = 0; i < n3; i += 3)
  {
    for (j = 0; j < m3; j += 3)
    {
      s00 = s01 = s02 = s10 = s11 = s12 = s20 = s21 = s22 = 0;

      for (k = 0; k < p; k++)
      {
        a0 = a[i * p + k];
        a1 = a[(i + 1) * p + k];
        a2 = a[(i + 2) * p + k];

        b0 = b[k * m + j];
        b1 = b[k * m + j + 1];
        b2 = b[k * m + j + 2];

        s00 += a0 * b0;
        s01 += a0 * b1;
        s02 += a0 * b2;

        s10 += a1 * b0;
        s11 += a1 * b1;
        s12 += a1 * b2;

        s20 += a2 * b0;
        s21 += a2 * b1;
        s22 += a2 * b2;
      }
      c[i * m + j] = s00;
      c[i * m + j + 1] = s01;
      c[i * m + j + 2] = s02;

      c[(i + 1) * m + j] = s10;
      c[(i + 1) * m + j + 1] = s11;
      c[(i + 1) * m + j + 2] = s12;

      c[(i + 2) * m + j] = s20;
      c[(i + 2) * m + j + 1] = s21;
      c[(i + 2) * m + j + 2] = s22;
    }

    for (; j < m; j++)
    {
      s00 = s10 = s20 = 0;
      for (k = 0; k < p; k++)
      {
        s00 += a[i * p + k] * b[k * m + j];
        s10 += a[(i + 1) * p + k] * b[k * m + j];
        s20 += a[(i + 2) * p + k] * b[k * m + j];
      }
      c[i * m + j] = s00;
      c[(i + 1) * m + j] = s10;
      c[(i + 2) * m + j] = s20;
    }
  }
  for (; i < n; i++)
  {
    for (j = 0; j < m; j++)
    {
      s = 0;
      for (k = 0; k < p; k++)
      {
        s += a[i * p + k] * b[k * m + j];
      }
      c[i * m + j] = s;
    }
  }
}

void multy_right(double *g, double *c, int v, int h, double *d)
{
  int i, j;
  double s;
  for (i = 0; i < v; i++)
  {
    s = 0;
    for (j = 0; j < h; j++)
    {
      s += g[i * h + j] * c[j];
    }
    d[i] = s;
  }
}

double
norm(double *a, int n)
{
  int i, j;
  double max = -1, s = 0;
  for (i = 0; i < n; i++)
  {
    s = 0;
    for (j = 0; j < n; j++)
    {
      s += fabs(a[i * n + j]);
    }
    if (max < s)
    {
      max = s;
    }
  }
  return max;
}

inline bool inverse(double *a, int n, double *c, double nrm_a)
{
  // const double nrm = norm(a, n);
  const double nrm = nrm_a;
  const double eps = EPS * nrm;
  double max, d;
  int i, j, k;
  int max_i;
  for (i = 0; i < n; ++i)
  {
    for (j = 0; j < n; ++j)
    {
      c[i * n + j] = (i == j) ? 1 : 0;
    }
  }

  for (i = 0; i < n; ++i)
  {
    max = fabs(a[i * n + i]);
    max_i = i;
    for (int k = i + 1; k < n; ++k)
    {
      if (fabs(a[k * n + i]) > max)
      {
        max = fabs(a[k * n + i]);
        max_i = k;
      }
    }

    if (fabs(a[max_i * n + i]) < eps)
    {
      return false;
    }

    if (max_i != i)
    {
      for (j = 0; j < n; ++j)
      {
        SWAP(a[i * n + j], a[max_i * n + j]);
        SWAP(c[i * n + j], c[max_i * n + j]);
      }
    }

    d = a[i * n + i];
    for (j = 0; j < n; ++j)
    {
      a[i * n + j] /= d;
      c[i * n + j] /= d;
    }

    for (k = 0; k < n; ++k)
    {
      if (k != i)
      {
        d = a[k * n + i];
        for (j = 0; j < n; ++j)
        {
          a[k * n + j] -= d * a[i * n + j];
          c[k * n + j] -= d * c[i * n + j];
        }
      }
    }
    // for (k = 0; k < n; ++k)
    // {
    //   if (k != i)
    //   {
    //     d = a[k * n + i];

    //     // Обработка блоками по 3 элемента
    //     for (j = 0; j < n - 2; j += 3)
    //     {
    //       a[k * n + j] -= d * a[i * n + j];
    //       a[k * n + j + 1] -= d * a[i * n + j + 1];
    //       a[k * n + j + 2] -= d * a[i * n + j + 2];

    //       c[k * n + j] -= d * c[i * n + j];
    //       c[k * n + j + 1] -= d * c[i * n + j + 1];
    //       c[k * n + j + 2] -= d * c[i * n + j + 2];
    //     }

    //     // Обработка остатка
    //     for (; j < n; ++j)
    //     {
    //       a[k * n + j] -= d * a[i * n + j];
    //       c[k * n + j] -= d * c[i * n + j];
    //     }
    //   }
    // }
  }

  return true;
}

inline bool main_element(double *a, int n, int m, int s, double *c, double *c_inv,
                         int &res_i, int &res_j, double nrm_a)
{
  // ищем блок с минимальной нормой обратной матрицы
  int k = n / m, i, j;
  double min = 0, nrm = 0;
  bool flag = true;
  for (i = s; i < k; i++)
  {
    for (j = s; j < k; j++)
    {
      get_block(a, n, m, i, j, c, m, m); // скопировали A_ij-ый блок в С
      if (inverse(c, m, c_inv, nrm_a))
      {
        nrm = norm(c_inv, m);
        // print (c_inv, m);
        if (flag)
        {
          min = nrm;
          res_i = i;
          res_j = j;

          flag = false;
        }
        else if (min > nrm)
        {
          min = nrm;
          res_i = i;
          res_j = j;
        }
      }
    }
  }
  return !flag;
}

inline void swap(double *a, int n, int m, double *b, int s, int i0,
                 int j0) // переставляет на s-ом шаге блочные строки s и i0 в матрице и
                         // правой части, столбцы s и j0 в матрице
{
  int i, j;
  if (i0 != s)
  {
    for (i = 0; i < m; i++)
    {
      for (j = 0; j < n - s * m; j++)
      {
        SWAP(a[n * (m * i0 + i) + m * s + j],
             a[n * (m * s + i) + m * s + j]);
      }
      SWAP(b[m * s + i], b[m * i0 + i]);
    }
  }
  if (j0 != s)
  {
    for (j = 0; j < m; j++)
    {
      for (i = 0; i < n; i++)
      {
        SWAP(a[m * j0 + n * i + j],
             a[m * s + n * i + j]);
      }
    }
  }
}

void add(double *a, double *b, int n, int m, double *c)
{
  int i, j;
  for (i = 0; i < n; i++)
  {
    for (j = 0; j < m; j++)
    {
      c[i * m + j] = a[i * m + j] + b[i * m + j];
    }
  }
}
void sub(double *a, double *b, int n, int m, double *c)
{
  int i, j;
  for (i = 0; i < n; i++)
  {
    for (j = 0; j < m; j++)
    {
      c[i * m + j] = a[i * m + j] - b[i * m + j];
    }
  }
}

bool is_zero(double *c, int v, int h, double nrm_a)
{
  bool fl = true;
  for (int q = 0; q < v * h; q++)
  {
    if (fabs(c[q]) >= EPS * nrm_a)
    {
      fl = false;
    }
  }
  return fl;
}

gauss_status gauss_method(int n, int m, double *a, double *b, double *x, double *c, double *g, double *d, double *f, int *p)
{
  int k, l, bl, v, h, i0 = 0, j0 = 0, v_g, h_g, h_d;
  int s, i, j; // для циклов
  double nrm_a = norm(a, n);
  if (fabs(nrm_a) < EPS_64) // нулевая матрица
  {
    return gauss_status::ZERO_MATRIX;
  }
  k = n / m;
  l = n % m;
  bl = (l != 0 ? k + 1 : k);

  for (i = 0; i < bl; i++)
  {
    p[i] = i;
  }
  (void)i0, (void)j0;
  for (s = 0; s < k; s++)
  {
    // printf("----------Step %d-----------\n", s);
    if (!main_element(a, n, m, s, c, g, i0, j0, nrm_a))
    {

      return gauss_status::NOT_APPLICABLE;
    }
    // print(a, n);
    // printf("i0 = %d, j0 = %d\n", i0, j0);
    swap(a, n, m, b, s, i0, j0);
    // print(a, n);
    // printf("\n");
    // print(b, 1, n);
    // printf("\n");
    std::swap(p[s], p[j0]);
    get_block(a, n, m, s, s, c, v, v); // с - квадратная, v = h
    inverse(c, v, g, nrm_a);
    // g - обратная к с
    // print(g, v);
    // printf("\n");
    // домножение строки на обратную матрицу
    for (i = s + 1; i < bl; i++)
    {
      get_block(a, n, m, s, i, c, v, h);
      // print(c, v, h);
      // printf("\n");
      multy(g, c, v, v, h, d);
      // print(d, v, h);
      // printf("\n");
      set_block(a, n, m, s, i, d, v, h);
    }
    // домножение правой части на обратную
    get_right(b, n, m, s, c, h);
    multy_right(g, c, v, h, d);
    set_right(b, m, s, d, h);

    // printf("матрица после умножения на обратную\n");
    // print(a, n);
    // printf("\n");
    // print(b, 1, n);
    // printf("\n");
    //  главный ход, делаем элементарные приобразования строк
    for (i = s + 1; i < bl; i++)
    {
      get_block(a, n, m, i, s, c, v, h);
      for (int q = 0; q < v * h; q++)
      {
        if (fabs(c[q]) < EPS_64)
        {
          c[q] = 0;
        }
      }
      // if (is_zero(c, v, h, nrm_a))
      // {
      //   continue;
      // }
      //   printf("c = ");
      //   printf("v = %d, h = %d, i = %d, s = %d\n", v, h, i, s);
      // print(c, v, h);
      // printf("\n");
      for (j = s + 1; j < bl; j++)
      {
        get_block(a, n, m, s, j, g, v_g, h_g);
        for (int q = 0; q < v_g * h_g; q++)
        {
          if (fabs(g[q]) < EPS_64)
          {
            g[q] = 0;
          }
        }
        // if (is_zero(g, v_g, h_g, nrm_a))
        // {
        //   continue;
        // }
        multy(c, g, v, h, h_g, d);
        get_block(a, n, m, i, j, g, v_g, h_g);
        sub(g, d, v, h_g, g);
        set_block(a, n, m, i, j, g, v, h_g);
      }
      // домножаем правую часть
      get_right(b, n, m, s, g, h_g);
      // print(g, 1, h_g);
      multy_right(c, g, v, h, d);
      // print(d, 1, v);
      get_right(b, n, m, i, g, h_g);
      // print(g, 1, h_g);
      sub(g, d, 1, h_g, f);
      // print(f, 1, h_g);
      set_right(b, m, i, f, h_g);
    }
    // print(a, n);
    // printf("\n");
    // print(b, 1, n);
    // printf("\n");
  }

  // обрабатываем правый нижний угловой блок размера l*l

  // printf("--------------------------------------------------------------------\n");
  if (l != 0)
  {
    s = bl - 1;
    get_block(a, n, m, s, s, c, v, v); // с - квадратная, v = h
    // printf("v = %d, s = %d\n", v, s);
    // print(c, v);
    // printf("\n");
    if (!inverse(c, v, g, nrm_a))
    {
      return NOT_APPLICABLE;
    }
    // g - обратная к с
    // print(g, v);
    get_right(b, n, m, s, c, h);
    // print(c, 1, h);
    multy_right(g, c, v, h, d);
    // print(d, 1, h);
    set_right(b, m, s, d, h);
    // print(a, n);
    // printf("\n");
    // print(b, 1, n);
  }

  // ---------- обратный ход -----------

  // for (i = 0; i < bl; i++)
  // {
  //   printf("p[%d] = %d\n", i, p[i]);
  // }

  // print(a, n);
  //  printf("\n");
  //  print(b, 1, n);
  //  printf("\n");
  get_right(b, n, m, bl - 1, c, h);
  set_right(x, m, p[bl - 1], c, h);
  // print(x, 1, n);
  // printf("\n");
  for (j = bl - 2; j >= 0; j--)
  {
    // printf("-------------------\n");
    for (i = 0; i < m * m; i++)
    {
      c[i] = 0;
    }
    for (i = 0; i < bl - j - 1; i++)
    {
      // printf("%d %d\n", j, bl - i - 1);
      get_block(a, n, m, j, bl - i - 1, g, v_g, h_g); // A_{i,(bl-i)} -> g
      get_right(x, n, m, p[bl - i - 1], d, h_d);      // X_(bl-i) -> d
      // print(g, v_g, h_g);
      // printf("\n");
      // print(d, 1, h_d);
      // printf("\n");
      multy_right(g, d, v_g, h_g, f);
      add(c, f, 1, v_g, c);
      // print(f, 1, v_g);
      // printf("\n");
      // print(c, 1, v_g);
    }
    // printf("\n");
    //  print(c, 1, v_g);
    get_right(b, n, m, j, g, h);
    // print(g, 1, h);
    sub(g, c, 1, h, g);
    // print(d, 1, h);
    set_right(x, m, p[j], g, h);
    // print(x, 1, n);
  }

  // print(x, 1, n);

  return gauss_status::DONE;
}
