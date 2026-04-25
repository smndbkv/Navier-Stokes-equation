#ifndef MAINSOLVER_H
#define MAINSOLVER_H

#include <stdio.h>
#include <random>
#include <string.h>
// #include <Eigen/Sparse>
// #include <Eigen/IterativeLinearSolvers>
#include "point.h"
#include "edge.h"
#include "grid.h"
#include "const.h"

#include "linearsystem/GaussianMethod/matrix.h"
#include "linearsystem/IterativeMethod/msr_matrix.h"

class MainSolver
{
public:
    int nx = 0, ny = 0, n = 0;
    double hx = 0, hy = 0;

    int nx_pressure = 0, ny_pressure = 0, n_pressure_edges = 0;
    int size = 0;
    point *points = nullptr;
    edge *edges = nullptr;
    grid grid_velocity;
    grid grid_pressure;
    FILE *gp = nullptr;
    int data_i = 0;

    double *matr = nullptr;
    double *A = nullptr;
    int *I = nullptr;
    const int L = 9;
    // Eigen::SparseMatrix<double> A;
    double *b = nullptr;
    // Eigen::VectorXd b;

    double *x_u = nullptr;
    double *x_f = nullptr;
    // Eigen::VectorXd x_u;
    // Eigen::VectorXd x_f;
    MainSolver(int nx, int ny, bool draw_flag = true, int s = 0)
    {
        this->nx = nx;
        this->ny = ny;
        this->n = nx * ny;
        hx = 1. / (nx - 1), hy = 1. / (ny - 1);

        nx_pressure = nx / 2 + 1;
        ny_pressure = ny / 2 + 1;
        n_pressure_edges = (nx_pressure - 1) * (ny_pressure - 1);
        size = (n + n + n_pressure_edges);

        matr = new double[size * size];
        memset(matr, 0, size * size * sizeof(double));
        A = new double[L + 1];
        I = new int[L + 1];
        // A.resize(n, n);
        // A.reserve(9 * n);
        b = new double[size];
        memset(b, 0, size * sizeof(double));
        // b.resize(n);
        x_u = new double[size];
        memset(x_u, 0, size * sizeof(double));

        x_f = new double[n];
        // x_u.resize(n);
        // x_f.resize(n);
        gp = popen("gnuplot -persist", "w");
        if (!gp)
        {
            perror("popen");
            return;
        }
        fprintf(gp, "set terminal wxt size 700,700\n");
        fprintf(gp, "set title 'Сетка'\n");
        fprintf(gp, "set xrange [-0.5:1.5]\n");
        fprintf(gp, "set yrange [-0.5:1.5]\n");
        if (draw_flag)
            fprintf(gp, "plot NaN notitle\n");
        // fflush(gp);
        grid_velocity.init(nx, ny, s);
        points = grid_velocity.points;
        edges = grid_velocity.edges;
        grid_pressure.init(nx_pressure, ny_pressure, s);
    }
    ~MainSolver()
    {
        nx = ny = 0;
        n = 0;
        delete[] matr;
        delete[] b;
        delete[] x_u;
        delete[] x_f;
        delete[] A;
        delete[] I;
        pclose(gp);
        gp = nullptr;
    }

    void draw_grid_velocity();
    void draw_grid_pressure();
    void draw_edge(edge *edges, int nx, int i, int j, double weight = 2, double color = 1);
    void draw_edge(edge &ed, double weight = 2, double color = 1);
    void draw_point(point &p);
    void draw_point_edges(int i, int j);
    void plot3d(double (MainSolver::*f)(point &p, int k, int l), int k, int l, double xmin, double xmax, int nx, double ymin, double ymax, int ny, const char *title = "3D plot");
    void plot3d(double (MainSolver::*f)(point &p), double xmin, double xmax, int nx, double ymin, double ymax, int ny, const char *title = "3D plot");
    void draw_phi(int k);
    void draw_phi_x(int k);
    void draw_u_exact();
    void draw_u_approximate();
    void draw_psi(int k);
    void visualizeFlow(double (MainSolver::*u)(point &p), double (MainSolver::*w)(point &p), double (MainSolver::*p)(point &p), int Np = 200, int Nvec = 20);

    edge linear_search(point &p)
    {
        edge ed;
        for (int i = 0; i < ny - 1; i++)
        {
            for (int j = 0; j < nx - 1; j++)
            {
                ed = edges[i * (nx - 1) + j];
                // if (ed.belongs(p))
                // {
                //     return ed;
                // }
                point polygon[4] = {*ed.points[0], *ed.points[1], *ed.points[2], *ed.points[3]};
                if (belongs_polygon(p, polygon, 4))
                {
                    return ed;
                }
            }
            // printf("\n");
        }
        return ed;
    }
    bool belongs_polygon(point &p, point *polygon, int n_pol)
    {
        bool has_positive = false;
        bool has_negative = false;

        for (int i = 0; i < n_pol; i++)
        {
            point v1 = polygon[i];
            point v2 = polygon[(i + 1) % n_pol];

            double cross = (v2.x - v1.x) * (p.y - v1.y) - (v2.y - v1.y) * (p.x - v1.x);

            if (cross > EPS)
                has_positive = true;
            if (cross < -EPS)
                has_negative = true;

            if (has_positive && has_negative)
                return false;
        }

        return true;
    }
    double plane(const point &p, const point &p0, const point &p1, const point &p2)
    {
        // Вычисляем знаменатель (ориентированная площадь треугольника p0-p1-p2, умноженная на 2)
        double denom = (p2.x - p0.x) * (p1.y - p0.y) - (p2.y - p0.y) * (p1.x - p0.x);
        // Если знаменатель близок к нулю, точки коллинеарны — решения нет
        double num = (p.x - p0.x) * (p1.y - p0.y) - (p.y - p0.y) * (p1.x - p0.x);

        return num / denom;
    }

    double phi(point &p, int k, int l = -1)
    {
        (void)l;
        point *p0, *p2;
        edge **eds = points[k].edges;
        for (int i = 0; i < 4; i++)
        {
            if (eds[i] != nullptr && eds[i]->belongs(p))
            {
                p0 = eds[i]->points[(i + 2) % 4]; // в этой точке функция равна 1
                p2 = eds[i]->points[(i + 4) % 4]; // в остальных ноль

                double a = 1 / (p0->x - p2->x);
                double b = -p2->x * a;
                double c = 1 / (p0->y - p2->y);
                double d = -p2->y * c;
                return (a * p.x + b) * (c * p.y + d);
            }
        }
        return 0;
    }

    double phi_x(point &p, int k, int l = -1)
    {
        (void)l;
        point *p0, *p2;
        edge **eds = points[k].edges;
        for (int i = 0; i < 4; i++)
        {
            if (eds[i] != nullptr && eds[i]->belongs(p))
            {
                p0 = eds[i]->points[(i + 2) % 4]; // в этой точке функция равна 1
                p2 = eds[i]->points[(i + 4) % 4]; // в остальных ноль

                double a = 1 / (p0->x - p2->x);
                double c = 1 / (p0->y - p2->y);
                double d = -p2->y * c;
                return a * (c * p.y + d);
            }
        }
        return 0;
    }
    double phi_y(point &p, int k, int l = -1)
    {
        (void)l;
        point *p0, *p2;
        edge **eds = points[k].edges;
        for (int i = 0; i < 4; i++)
        {
            if (eds[i] != nullptr && eds[i]->belongs(p))
            {
                p0 = eds[i]->points[(i + 2) % 4]; // в этой точке функция равна 1
                p2 = eds[i]->points[(i + 4) % 4]; // в остальных ноль

                double a = 1 / (p0->x - p2->x);
                double b = -p2->x * a;
                double c = 1 / (p0->y - p2->y);
                return (a * p.x + b) * c;
            }
        }
        return 0;
    }

    double psi(point &p, int k, int l = -1)
    {
        (void)l;
        if (grid_pressure.edges[k].belongs(p))
        {
            return 1;
        }
        return 0;
    }

    double integral_triangle(double (MainSolver::*f)(point &p, int k), int k, point &p0, point &p1, point &p2)
    {
        double s = 0.5 * fabs((p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y));
        point m0, m1, m2;
        m0.init(0.5 * (p0.x + p1.x), 0.5 * (p0.y + p1.y));
        m1.init(0.5 * (p1.x + p2.x), 0.5 * (p1.y + p2.y));
        m2.init(0.5 * (p2.x + p0.x), 0.5 * (p2.y + p0.y));
        return (1. / 3) * s * ((this->*f)(m0, k) + (this->*f)(m1, k) + (this->*f)(m2, k));
    }

    double gauss2x2_integral(double (MainSolver::*f)(point &p, int k, int l), int k, int l, double xmin, double ymin, double xmax, double ymax)
    {
        const double sqrt3 = std::sqrt(3.0);
        const double u = 1.0 / sqrt3; // узлы в опорном квадрате [-1,1]

        // центр и половины длин сторон
        double xc = (xmin + xmax) * 0.5;
        double yc = (ymin + ymax) * 0.5;
        double hx = (xmax - xmin) * 0.5;
        double hy = (ymax - ymin) * 0.5;

        double J = hx * hy; // якобиан преобразования (площадь/4)

        // сумма значений в четырёх узлах
        double sum = 0.0;
        point p;
        p.init(xc - hx * u, yc - hy * u);
        sum += (this->*f)(p, k, l);
        p.init(xc + hx * u, yc - hy * u);
        sum += (this->*f)(p, k, l);
        p.init(xc - hx * u, yc + hy * u);
        sum += (this->*f)(p, k, l);
        p.init(xc + hx * u, yc + hy * u);
        sum += (this->*f)(p, k, l);

        return J * sum;
    }
    double integral(double (MainSolver::*f)(point &p, int k, int l), int k, int l)
    {
        point *p0, *p1;
        edge **eds = points[k].edges;
        double sum = 0;
        for (int i = 0; i < 4; i++)
        {
            if (eds[i] != nullptr)
            {
                p0 = eds[i]->points[3];
                p1 = eds[i]->points[1];
                sum += gauss2x2_integral(f, k, l, p0->x, p0->y, p1->x, p1->y);
            }
        }
        return sum;
    }
    double phi_square(point &p, int k)
    {
        return phi(p, k) * phi(p, k);
    }
    // double integral_phi(int k)
    // {
    //     printf("integral(phi) = %lf\n", integral(&grid::phi, k));
    //     return 0;
    // }

    /// ВАЖНО писать в начале ту функцию у которой носитель меньше, то есть phi, phi_x, phi_y
    double integral_phi_square(int k)
    {
        printf("integral(phi^2) = %lf\n", integral(&MainSolver::phi_kl, k, k));
        return 0;
    }
    double phi_kl(point &p, int k, int l)
    {
        return phi(p, k) * phi(p, l);
    }
    double phi_x_kl(point &p, int k, int l)
    {
        return phi_x(p, k) * phi_x(p, l);
    }
    double phi_y_kl(point &p, int k, int l)
    {
        return phi_y(p, k) * phi_y(p, l);
    }
    double phi_x_psi(point &p, int k, int l)
    {
        return phi_x(p, k) * psi(p, l);
    }
    double phi_y_psi(point &p, int k, int l)
    {
        return phi_y(p, k) * psi(p, l);
    }
    void init_matr()
    {
        memset(matr, 0, n * n * sizeof(double));
        int ind_l[9];
        int l;
        int i, j;
        for (int k = 0; k < n; k++) // на границе оставляем нули
        {
            i = k / nx, j = k % nx;
            if (i == 0 || i == ny - 1 || j == 0 || j == nx - 1)
            {
                continue;
            }
            b[k] = integral(&MainSolver::f1_phi_k, k, k);
            b[k + n] = integral(&MainSolver::f2_phi_k, k, k);
            ind_l[0] = k - nx - 1;
            ind_l[1] = k - nx;
            ind_l[2] = k - nx + 1;
            ind_l[3] = k - 1;
            ind_l[4] = k;
            ind_l[5] = k + 1;
            ind_l[6] = k + nx - 1;
            ind_l[7] = k + nx;
            ind_l[8] = k + nx + 1;
            for (l = 0; l < 9; l++)
            {
                if (ind_l[l] >= 0)
                    break;
            }
            for (; l < 9 && ind_l[l] < n; l++)
            {
                i = ind_l[l] / nx, j = ind_l[l] % nx;
                if (i == 0 || i == ny - 1 || j == 0 || j == nx - 1)
                {
                    continue;
                }
                matr[k * size + ind_l[l]] = integral(&MainSolver::phi_x_kl, k, ind_l[l]) + integral(&MainSolver::phi_y_kl, k, ind_l[l]);
                matr[(k + n) * size + ind_l[l] + n] = matr[k * size + ind_l[l]];
            }
        }
        for (int k = 0; k < n; k++)
        {
            int i = k / nx, j = k % nx;
            if (i == 0 || i == ny - 1 || j == 0 || j == nx - 1)
            {
                matr[(k + n) * size + k + n] = matr[k * size + k] = 1;
                b[k + n] = b[k] = 0;
            }
        }
        for (int k = 0; k < n_pressure_edges; k++)
        {
            for (int l = 0; l < n; l++)
            {

                matr[(k + 2 * n) * size + l] = integral(&MainSolver::phi_x_psi, l, k);
                matr[(k + 2 * n) * size + l + n] = integral(&MainSolver::phi_y_psi, l, k);
                matr[l * size + (k + 2 * n)] = -matr[(k + 2 * n) * size + l];
                matr[(l + n) * size + (k + 2 * n)] = -matr[(k + 2 * n) * size + l + n];
            }
        }
    }
    bool is_symmetric(double eps = EPS)
    {
        for (int i = 0; i < 2 * n; i++)
        {
            for (int j = i + 1; j < 2 * n; j++)
            {
                double a_ij = matr[i * size + j];
                double a_ji = matr[j * size + i];
                if (std::fabs(a_ij - a_ji) > eps)
                {
                    printf("matr[%d, %d] = %10.3e, matr[%d, %d] = %10.3e\n", i, j, a_ij, j, i, a_ji);
                    return false;
                }
            }
        }
        return true;
    }
    bool is_skew_symmetric(double eps = EPS)
    {
        for (int i = 2 * n; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                double a_ij = matr[i * size + j];
                double a_ji = -matr[j * size + i];
                if (std::fabs(a_ij - a_ji) > eps)
                {
                    printf("matr[%d, %d] = %10.3e, matr[%d, %d] = %10.3e\n", i, j, a_ij, j, i, -a_ji);
                    return false;
                }
            }
        }
        return true;
    }
    void init_D_x()
    {
        for (int k = 0; k < n_pressure_edges; k++)
        {
            for (int l = 0; l < n; l++)
            {
                printf("%10.3e ", integral(&MainSolver::phi_x_psi, l, k));
            }
            printf("\n");
        }
    }
    void init_D_y()
    {
        for (int k = 0; k < n_pressure_edges; k++)
        {
            for (int l = 0; l < n; l++)
            {
                printf("%10.3e ", integral(&MainSolver::phi_y_psi, l, k));
            }
            printf("\n");
        }
    }

    void init_matr_m_f_approximate()
    {
        memset(matr, 0, n * n * sizeof(double));

        int ind_l[9];
        int l;
        for (int k = 0; k < n; k++) // на границе оставляем нули
        {
            b[k] = integral(&MainSolver::f1_phi_k, k, k);

            ind_l[0] = k - nx - 1;
            ind_l[1] = k - nx;
            ind_l[2] = k - nx + 1;
            ind_l[3] = k - 1;
            ind_l[4] = k;
            ind_l[5] = k + 1;
            ind_l[6] = k + nx - 1;
            ind_l[7] = k + nx;
            ind_l[8] = k + nx + 1;
            for (l = 0; l < 9; l++)
            {
                if (ind_l[l] >= 0)
                    break;
            }
            for (; l < 9 && ind_l[l] < n; l++)
            {
                matr[k * n + ind_l[l]] = integral(&MainSolver::phi_kl, k, ind_l[l]);
            }
        }
    }
    void init_msr_matrix()
    {

        I[0] = n + 1;
        int ii = 0;
        int ind_l[9];
        int l;
        int J;
        int i, j;
        for (int k = 1; k < n; k++) // на границе оставляем нули
        {
            i = k / nx, j = k % nx;
            if (i == 0 || i == ny - 1 || j == 0 || j == nx - 1)
            {
                continue;
            }
            ind_l[0] = k - nx - 1;
            ind_l[1] = k - nx;
            ind_l[2] = k - nx + 1;
            ind_l[3] = k - 1;
            ind_l[4] = k;
            ind_l[5] = k + 1;
            ind_l[6] = k + nx - 1;
            ind_l[7] = k + nx;
            ind_l[8] = k + nx + 1;
            for (l = 0; l < 9; l++)
            {
                if (ind_l[l] >= 0)
                    break;
            }
            ii = 0;
            for (; l < 9 && ind_l[l] < n; l++)
            {
                i = ind_l[l] / nx, j = ind_l[l] % nx;
                if (i == 0 || i == ny - 1 || j == 0 || j == nx - 1)
                {
                    continue;
                }
                ii++;
            }
            I[i] = ii + I[i - 1];
        }
        I[n] = 3 + I[n - 1];

        for (int k = 0; k < n; k++) // на границе оставляем нули
        {
            i = k / nx, j = k % nx;
            if (i == 0 || i == ny - 1 || j == 0 || j == nx - 1)
            {
                continue;
            }
            ind_l[0] = k - nx - 1;
            ind_l[1] = k - nx;
            ind_l[2] = k - nx + 1;
            ind_l[3] = k - 1;
            ind_l[4] = k;
            ind_l[5] = k + 1;
            ind_l[6] = k + nx - 1;
            ind_l[7] = k + nx;
            ind_l[8] = k + nx + 1;
            J = I[i];
            int jj = 0;
            for (; l < 9 && ind_l[l] < n; l++)
            {
                i = ind_l[l] / nx, j = ind_l[l] % nx;
                if (i == 0 || i == ny - 1 || j == 0 || j == nx - 1)
                {
                    continue;
                }
                if (l != 4)
                {
                    I[J + jj] = ind_l[l];
                    jj++;
                }
            }
            for (l = 0; l < 9; l++)
            {
                if (ind_l[l] >= 0)
                    break;
            }
            for (; l < 9 && ind_l[l] < n; l++)
            {
                i = ind_l[l] / nx, j = ind_l[l] % nx;
                if (i == 0 || i == ny - 1 || j == 0 || j == nx - 1)
                {
                    continue;
                }
                set(A, I, n, k, ind_l[l], integral(&MainSolver::phi_kl, k, ind_l[l]) + integral(&MainSolver::phi_x_kl, k, ind_l[l]) + integral(&MainSolver::phi_y_kl, k, ind_l[l]));
            }
        }
        for (int k = 0; k < n; k++)
        {
            int i = k / nx, j = k % nx;
            if (i == 0 || i == ny - 1 || j == 0 || j == nx - 1)
            {
                A[k] = 1;
            }
        }
    }
    void print_A()
    {
        print_msr_matrix(A, I, n);
    }
    void print_matr()
    {
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                printf(" %10.3e", matr[i * size + j]);
            }
            printf("\n");
        }
    }
    void print_b()
    {
        for (int i = 0; i < size; i++)
        {
            printf(" %10.3e", b[i]);
        }
        printf("\n");
    }
    void print_x()
    {
        for (int i = 0; i < size; i++)
        {
            printf(" %10.3e", x_u[i]);
        }
        printf("\n");
    }
    double f1_phi_k(point &p, int k, int)
    {
        return f1(p) * phi(p, k);
    }
    double f2_phi_k(point &p, int k, int)
    {
        return f2(p) * phi(p, k);
    }
    // double f_approximate_phi_k(point &p, int k, int)
    // {
    //     return f_approximate(p) * phi(p, k);
    // }
    void solve_linear_system()
    {
        // init_matr();
        const int m = 60;
        double *c = new double[m * m], *g = new double[m * m],
               *d = new double[m * m], *f = new double[m * m];
        int p_size = n;
        int *p = new int[p_size];
        memset(c, 0, m * m * sizeof(double));
        memset(g, 0, m * m * sizeof(double));
        memset(d, 0, m * m * sizeof(double));
        memset(f, 0, m * m * sizeof(double));
        memset(p, 0, p_size * sizeof(int));

        gauss_method(size, m, matr, b, x_u, c, g, d, f, p);
        delete[] c;
        delete[] g;
        delete[] d;
        delete[] f;
        delete[] p;
    }
    void solve_linear_system_f()
    {
        // init_matr_m_f_approximate();
        const int m = 60;
        double *c = new double[m * m], *g = new double[m * m],
               *d = new double[m * m], *f = new double[m * m];
        int *p = new int[(n % m != 0 ? n / m + 1 : n / m)];
        gauss_method(n, m, matr, b, x_f, c, g, d, f, p);
        delete[] c;
        delete[] g;
        delete[] d;
        delete[] f;
        delete[] p;
    }
    // double f_approximate(point &p, int k = -1, int l = -1)
    // {
    //     (void)k, (void)l;
    //     double sum = 0;
    //     for (int k = 0; k < n; k++)
    //     {
    //         sum += x_f[k] * phi(p, k);
    //     }
    //     return sum;
    // }
    void draw_f1()
    {
        plot3d(&MainSolver::f1_test, 0, 1, 60, 0, 1, 60);
    }
    void draw_f2()
    {
        plot3d(&MainSolver::f2_test, 0, 1, 60, 0, 1, 60);
    }
    // void draw_f_approximate()
    // {
    //     plot3d(&MainSolver::f_approximate, -1, -1, 0, 1, 60, 0, 1, 60);
    // }
    double get_residual_c(double (MainSolver::*f_exact)(point &p), double (MainSolver::*f_approximate)(point &p))
    {
        double max = -1, d;
        int nx3 = 3 * nx, ny3 = 3 * ny;
        point p;
        for (int i = 0; i < nx3; i++)
        {
            for (int j = 0; j < ny3; j++)
            {
                p.init((double)j / (nx3 - 1), (double)(ny3 - 1 - i) / (ny3 - 1));
                d = fabs((this->*f_exact)(p) - (this->*f_approximate)(p));
                if (max < d)
                {
                    max = d;
                }
            }
        }
        return max;
    }
    double get_residual_l1(double (MainSolver::*f_exact)(point &p), double (MainSolver::*f_approximate)(point &p))
    {
        double sum = 0;
        int nx3 = 3 * nx, ny3 = 3 * ny;
        double hx3 = 1. / (nx3 - 1), hy3 = 1. / (ny3 - 1);
        point p;
        for (int i = 0; i < nx3; i++)
        {
            for (int j = 0; j < ny3; j++)
            {
                p.init((double)j / (nx3 - 1), (double)(ny3 - 1 - i) / (ny3 - 1));
                sum += fabs((this->*f_exact)(p) - (this->*f_approximate)(p)) * hx3 * hy3;
            }
        }
        return sum;
    }
    // double get_r_f()
    // {
    //     double max = -1, d;
    //     int nx3 = 3 * nx, ny3 = 3 * ny;
    //     point p;
    //     for (int i = 0; i < nx3; i++)
    //     {
    //         for (int j = 0; j < ny3; j++)
    //         {
    //             p.init((double)j / (nx3 - 1), (double)(ny3 - 1 - i) / (ny3 - 1));
    //             d = fabs(f2_test(p) - f_approximate(p));
    //             if (max < d)
    //             {
    //                 max = d;
    //             }
    //         }
    //     }
    //     return max;
    // }
    double f1(point &p);
    double f2(point &p);
    double u_exact(point &p);
    double w_exact(point &p);
    double p_exact(point &p);
    double f1_test0(point &p);
    double f2_test0(point &p);
    double u_test0(point &p);
    double w_test0(point &p);
    double p_test0(point &p);
    double f1_test1(point &p);
    double f2_test1(point &p);
    double u_test1(point &p);
    double w_test1(point &p);
    double p_test1(point &p);
    double f1_test2(point &p);
    double f2_test2(point &p);
    double u_test2(point &p);
    double w_test2(point &p);
    double p_test2(point &p);
    double p_test2_(point &p);

    double f1_test(point &p);
    double f2_test(point &p);
    double f3_test(point &p);

    double u1_exact(point &p);
    double u2_exact(point &p);
    double u3_exact(point &p);

    double u_approximate(point &p)
    {
        double sum = 0;
        for (int k = 0; k < n; k++)
        {
            sum += x_u[k] * phi(p, k);
        }
        return sum;
    }
    double w_approximate(point &p)
    {
        double sum = 0;
        for (int k = 0; k < n; k++)
        {
            sum += x_u[k + n] * phi(p, k);
        }
        return sum;
    }
    double p_approximate(point &p)
    {
        double sum = 0;
        for (int k = 0; k < n_pressure_edges; k++)
        {
            sum += x_u[k + 2 * n] * psi(p, k);
        }
        return sum;
    }
    double err_u(point &p)
    {
        return u_exact(p) - u_approximate(p);
    }
    double err_w(point &p)
    {
        return w_exact(p) - w_approximate(p);
    }
    double err_p(point &p)
    {
        return p_exact(p) - p_approximate(p);
    }
    double u0(point &p);
};

#endif