#ifndef GRID_H
#define GRID_H

#include <stdio.h>
#include <random>
#include <string.h>
// #include <Eigen/Sparse>
// #include <Eigen/IterativeLinearSolvers>
#include "point.h"
#include "const.h"
#include "linearsystem/matrix.h"

class grid
{
public:
    int nx = 0, ny = 0;
    point *points = nullptr;
    edge *edges = nullptr;
    FILE *gp = nullptr;
    int data_i = 0;

    double *matr = nullptr;
    // Eigen::SparseMatrix<double> A;
    int n = 0;
    double *b = nullptr;
    // Eigen::VectorXd b;

    double *x_u = nullptr;
    double *x_f = nullptr;
    // Eigen::VectorXd x_u;
    // Eigen::VectorXd x_f;
    grid(int nx, int ny, bool draw_flag = true, int s = 0)
    {
        this->nx = nx;
        this->ny = ny;
        this->n = nx * ny;
        points = new point[nx * ny];
        edges = new edge[(nx - 1) * (ny - 1)];
        matr = new double[n * n];
        memset(matr, 0, n * n * sizeof(double));
        // A.resize(n, n);
        // A.reserve(9 * n);
        b = new double[n];
        // b.resize(n);
        x_u = new double[n];
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
        for (int i = 0; i < ny; i++)
        {
            for (int j = 0; j < nx; j++)
            {
                points[i * nx + j].init(f_x(i, j, s), f_y(i, j, s), i * nx + j);
                // printf(" (%.3lf, %.3lf)", points[i * nx + j].x, points[i * nx + j].y);
            }
            // printf("\n");
        }
        for (int i = 0; i < ny - 1; i++)
        {
            for (int j = 0; j < nx - 1; j++)
            {
                edges[i * (nx - 1) + j].init(points + i * nx + j, points + i * nx + j + 1, points + (i + 1) * nx + j + 1, points + (i + 1) * nx + j, i * (nx - 1) + j);
            }
            // printf("\n");
        }
        // устанавливаем грани которым принадлежит точка

        points[0].edges[2] = edges;
        for (int j = 1; j < nx - 1; j++)
        {
            points[j].edges[2] = edges + j;
            points[j].edges[3] = edges + (j - 1);
        }
        points[nx - 1].edges[3] = edges + nx - 2;

        for (int i = 1; i < ny - 1; i++)
        {
            points[i * nx].edges[1] = edges + (i - 1) * (nx - 1);
            points[i * nx].edges[2] = edges + i * (nx - 1);

            for (int j = 1; j < nx - 1; j++)
            {
                points[i * nx + j].edges[0] = edges + (i - 1) * (nx - 1) + (j - 1);
                points[i * nx + j].edges[1] = edges + (i - 1) * (nx - 1) + j;
                points[i * nx + j].edges[2] = edges + i * (nx - 1) + j;
                points[i * nx + j].edges[3] = edges + i * (nx - 1) + (j - 1);
            }
            points[i * nx + (nx - 1)].edges[0] = edges + (i - 1) * (nx - 1) + nx - 2;
            points[i * nx + (nx - 1)].edges[3] = edges + i * (nx - 1) + nx - 2;
        }

        points[(ny - 1) * nx].edges[1] = edges + (ny - 2) * (nx - 1);
        for (int j = 1; j < nx - 1; j++)
        {
            points[(ny - 1) * nx + j].edges[0] = edges + (ny - 2) * (nx - 1) + (j - 1);
            points[(ny - 1) * nx + j].edges[1] = edges + (ny - 2) * (nx - 1) + j;
        }
        points[(ny - 1) * nx + (nx - 1)].edges[0] = edges + (ny - 2) * (nx - 1) + (nx - 2);
    }
    ~grid()
    {
        nx = ny = 0;
        n = 0;
        delete[] points;
        delete[] edges;
        delete[] matr;
        delete[] b;
        delete[] x_u;
        delete[] x_f;
        pclose(gp);
        gp = nullptr;
    }
    double f_x(int, int j, int s = 0)
    {
        switch (s)
        {
        case 0:
            return (double)j / (nx - 1);
        default:
            return 0;
        }
    }

    double f_y(int i, int, int s = 0)
    {
        switch (s)
        {
        case 0:
            return (double)(ny - 1 - i) / (ny - 1);
        default:
            return 0;
        }
    }

    void draw_grid()
    {
        for (int i = 0; i < ny - 1; i++)
        {
            for (int j = 0; j < nx - 1; j++)
            {
                draw_edge(i, j, 0.7, 0);
            }
        }
    }
    void draw_edge(int i, int j, double weight = 2, double color = 1)
    {
        edge ed = edges[i * (nx - 1) + j];
        point *p0 = ed.points[0], *p1 = ed.points[1], *p2 = ed.points[2], *p3 = ed.points[3];

        // fprintf(gp, "set grid\n");

        // Передаём данные в виде блока и рисуем замкнутую линию
        fprintf(gp, "$data%d << EOD\n", data_i);
        fprintf(gp, "%lf %lf\n", p0->x, p0->y);
        fprintf(gp, "%lf %lf\n", p1->x, p1->y);
        fprintf(gp, "%lf %lf\n", p2->x, p2->y);
        fprintf(gp, "%lf %lf\n", p3->x, p3->y);
        fprintf(gp, "%lf %lf\n", p0->x, p0->y);
        fprintf(gp, "EOD\n");

        fprintf(gp, "replot $data%d with lines lw %lf lc %lf notitle \n", data_i, weight, color);
        data_i++;
        // fflush(gp);
    }
    void draw_edge(edge &ed, double weight = 2, double color = 1)
    {
        point *p0 = ed.points[0], *p1 = ed.points[1], *p2 = ed.points[2], *p3 = ed.points[3];

        // fprintf(gp, "set grid\n");

        // Передаём данные в виде блока и рисуем замкнутую линию
        fprintf(gp, "$data%d << EOD\n", data_i);
        fprintf(gp, "%lf %lf\n", p0->x, p0->y);
        fprintf(gp, "%lf %lf\n", p1->x, p1->y);
        fprintf(gp, "%lf %lf\n", p2->x, p2->y);
        fprintf(gp, "%lf %lf\n", p3->x, p3->y);
        fprintf(gp, "%lf %lf\n", p0->x, p0->y);
        fprintf(gp, "EOD\n");

        fprintf(gp, "replot $data%d with lines lw %lf lc %lf notitle \n", data_i, weight, color);
        data_i++;
        // fflush(gp);
    }
    void draw_point(point &p)
    {
        fprintf(gp, "$data%d << EOD\n", data_i);
        fprintf(gp, "%lf %lf\n", p.x, p.y);
        fprintf(gp, "EOD\n");
        fprintf(gp, "replot $data%d with points pt 7 ps 1 lc 'red' notitle\n", data_i);
        data_i++;
    }
    void draw_point_edges(int i, int j)
    {
        edge **eds = points[i * nx + j].edges;
        for (int r = 0; r < 4; r++)
        {
            if (eds[r] != nullptr)
                draw_edge(*eds[r], 3, 0);
        }
    }
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

    double integral_triangle(double (grid::*f)(point &p, int k), int k, point &p0, point &p1, point &p2)
    {
        double s = 0.5 * fabs((p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y));
        point m0, m1, m2;
        m0.init(0.5 * (p0.x + p1.x), 0.5 * (p0.y + p1.y));
        m1.init(0.5 * (p1.x + p2.x), 0.5 * (p1.y + p2.y));
        m2.init(0.5 * (p2.x + p0.x), 0.5 * (p2.y + p0.y));
        return (1. / 3) * s * ((this->*f)(m0, k) + (this->*f)(m1, k) + (this->*f)(m2, k));
    }

    double gauss2x2_integral(double (grid::*f)(point &p, int k, int l), int k, int l, double xmin, double ymin, double xmax, double ymax)
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
    double integral(double (grid::*f)(point &p, int k, int l), int k, int l)
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
    double integral_phi_square(int k)
    {
        printf("integral(phi^2) = %lf\n", integral(&grid::phi_kl, k, k));
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
    void plot3d(double (grid::*f)(point &p, int k, int l), int k, int l, double xmin, double xmax, int nx, double ymin, double ymax, int ny, const char *title = "3D plot")
    {
        // Открываем pipe в gnuplot, сохраняем указатель в gp3d
        FILE *gp3d = popen("gnuplot -persist", "w");
        if (!gp3d)
        {
            perror("popen failed");
            return;
        }

        // Настройки графика
        fprintf(gp3d, "set terminal qt enhanced font 'Verdana,10'\n");
        if (title)
        {
            fprintf(gp3d, "set title '%s'\n", title);
        }
        fprintf(gp3d, "set xlabel 'x'\n");
        fprintf(gp3d, "set ylabel 'y'\n");
        fprintf(gp3d, "set zlabel 'z'\n");
        fprintf(gp3d, "set grid\n");
        fprintf(gp3d, "set hidden3d\n");
        fprintf(gp3d, "set dgrid3d %d,%d\n", nx, ny);
        fprintf(gp3d, "set view 60,30\n");

        // Команда построения с встроенными данными
        fprintf(gp3d, "splot '-' with lines title 'f(x,y)'\n");

        // Генерация данных
        double dx = (nx > 1) ? (xmax - xmin) / (nx - 1) : 0.0;
        double dy = (ny > 1) ? (ymax - ymin) / (ny - 1) : 0.0;

        for (int j = 0; j < ny; ++j)
        {
            double y = ymin + j * dy;
            for (int i = 0; i < nx; ++i)
            {
                double x = xmin + i * dx;
                point p = {x, y};
                double z = (this->*f)(p, k, l);
                fprintf(gp3d, "%f %f %f\n", x, y, z);
            }
            fprintf(gp3d, "\n"); // разделитель блоков
        }

        fprintf(gp3d, "e\n"); // конец данных
        fflush(gp3d);         // принудительная отправка

        pclose(gp3d); // закрытие pipe
    }
    void draw_phi(int k)
    {
        plot3d(&grid::phi, k, k, 0, 1, 60, 0, 1, 60);
    }
    void draw_phi_x(int k)
    {
        plot3d(&grid::phi_x, k, k, 0, 1, 60, 0, 1, 60);
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
            b[k] = integral(&grid::f_phi_k, k, k);

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
                matr[k * n + ind_l[l]] = integral(&grid::phi_kl, k, ind_l[l]) + integral(&grid::phi_x_kl, k, ind_l[l]) + integral(&grid::phi_y_kl, k, ind_l[l]);
            }
        }
        for (int k = 0; k < n; k++)
        {
            int i = k / nx, j = k % nx;
            if (i == 0 || i == ny - 1 || j == 0 || j == nx - 1)
            {

                matr[k * n + k] = 1;
            }
        }
    }
    void init_matr_m_f_approximate()
    {
        memset(matr, 0, n * n * sizeof(double));

        int ind_l[9];
        int l;
        for (int k = 0; k < n; k++) // на границе оставляем нули
        {
            b[k] = integral(&grid::f_phi_k, k, k);

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
                matr[k * n + ind_l[l]] = integral(&grid::phi_kl, k, ind_l[l]);
            }
        }
    }
    void print_matr()
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                printf(" %10.3e", matr[i * n + j]);
            }
            printf("\n");
        }
    }

    double f1(point &p, int k = -1, int l = -1)
    {
        (void)k, (void)l;
        double x = p.x, y = p.y;
        return 2 * (y * (1 - y) + x * (1 - x));
    }
    double f2(point &p, int k = -1, int l = -1)
    {
        (void)k, (void)l;
        double x = p.x, y = p.y;
        return 13 * M_PI * M_PI * sin(2 * M_PI * x) * sin(3 * M_PI * y);
    }
    double f_phi_k(point &p, int k, int)
    {
        return f2(p) * phi(p, k);
    }
    double f_approximate_phi_k(point &p, int k, int)
    {
        return f_approximate(p) * phi(p, k);
    }
    void solve_linear_system()
    {
        // init_matr();
        const int m = 60;
        double *c = new double[m * m], *g = new double[m * m],
               *d = new double[m * m], *f = new double[m * m];
        int *p = new int[(n % m != 0 ? n / m + 1 : n / m)];
        gauss_method(n, m, matr, b, x_u, c, g, d, f, p);
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
    double f_approximate(point &p, int k = -1, int l = -1)
    {
        (void)k, (void)l;
        double sum = 0;
        for (int k = 0; k < n; k++)
        {
            sum += x_f[k] * phi(p, k);
        }
        return sum;
    }
    void draw_f1()
    {
        plot3d(&grid::f1, -1, -1, 0, 1, 60, 0, 1, 60);
    }
    void draw_f2()
    {
        plot3d(&grid::f2, -1, -1, 0, 1, 60, 0, 1, 60);
    }
    void draw_f_approximate()
    {
        plot3d(&grid::f_approximate, -1, -1, 0, 1, 60, 0, 1, 60);
    }
    double get_r_u()
    {
        double max = -1, d;
        int nx3 = 3 * nx, ny3 = 3 * ny;
        point p;
        for (int i = 0; i < nx3; i++)
        {
            for (int j = 0; j < ny3; j++)
            {
                p.init((double)j / (nx3 - 1), (double)(ny3 - 1 - i) / (ny3 - 1));
                d = fabs(u2(p) - u_approximate(p));
                if (max < d)
                {
                    max = d;
                }
            }
        }
        return max;
    }
    double get_r_f()
    {
        double max = -1, d;
        int nx3 = 3 * nx, ny3 = 3 * ny;
        point p;
        for (int i = 0; i < nx3; i++)
        {
            for (int j = 0; j < ny3; j++)
            {
                p.init((double)j / (nx3 - 1), (double)(ny3 - 1 - i) / (ny3 - 1));
                d = fabs(f2(p) - f_approximate(p));
                if (max < d)
                {
                    max = d;
                }
            }
        }
        return max;
    }
    double u1(point &p, int k = -1, int l = -1)
    {
        (void)k, (void)l;
        double x = p.x, y = p.y;
        return x * (1 - x) * y * (1 - y);
    }
    double u2(point &p, int k = -1, int l = -1)
    {
        (void)k, (void)l;
        double x = p.x, y = p.y;
        return sin(2 * M_PI * x) * sin(3 * M_PI * y);
    }
    double u_approximate(point &p, int k = -1, int l = -1)
    {
        (void)k, (void)l;
        double sum = 0;
        for (int k = 0; k < n; k++)
        {
            sum += x_u[k] * phi(p, k);
        }
        return sum;
    }
    void draw_u1()
    {
        plot3d(&grid::u1, -1, -1, 0, 1, 60, 0, 1, 60, "u exact");
    }
    void draw_u2()
    {
        plot3d(&grid::u2, -1, -1, 0, 1, 60, 0, 1, 60, "u exact");
    }
    void draw_u_approximate()
    {
        plot3d(&grid::u_approximate, -1, -1, 0, 1, 60, 0, 1, 60, "u approximate");
    }
};

#endif