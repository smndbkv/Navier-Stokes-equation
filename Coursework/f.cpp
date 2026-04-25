#include "MainSolver.h"

double MainSolver::f1(point &p) // правая часть
{
    return f1_test2(p);
}
double MainSolver::f2(point &p) // правая часть
{
    return f2_test2(p);
}

double MainSolver::u_exact(point &p)
{
    return u_test2(p);
}
double MainSolver::w_exact(point &p)
{
    return w_test2(p);
}
double MainSolver::p_exact(point &p)
{
    return p_test2(p);
}

double MainSolver::u0(point &p) // значения на границе
{
    (void)p;
    return 0;
}

double MainSolver::f1_test0(point &p)
{
    double x = p.x, y = p.y;
    return (1 - 2 * x) * y * (1 - y);
}
double MainSolver::f2_test0(point &p)
{
    double x = p.x, y = p.y;
    return (1 - 2 * y) * x * (1 - x);
}
double MainSolver::u_test0(point &p)
{
    (void)p;
    return 0;
}
double MainSolver::w_test0(point &p)
{
    (void)p;
    return 0;
}
double MainSolver::p_test0(point &p)
{
    double x = p.x, y = p.y;

    return x * (1 - x) * y * (1 - y);
}

double MainSolver::f1_test1(point &p)
{
    double x = p.x, y = p.y;
    // A2''(x) = 2 - 12x + 12x^2
    double A2_xx = 2.0 - 12.0 * x + 12.0 * x * x;
    // C(y) = y - 3y^2 + 2y^3
    double C = y - 3.0 * y * y + 2.0 * y * y * y;
    // A2(x) = x^2 - 2x^3 + x^4
    double A2 = x * x - 2.0 * x * x * x + x * x * x * x;
    // C''(y) = -6 + 12y
    double C_yy = -6.0 + 12.0 * y;
    double laplace_u = 2.0 * (A2_xx * C + A2 * C_yy);
    return -laplace_u + 2.0 * x;
}
double MainSolver::f2_test1(point &p)
{
    double x = p.x, y = p.y;
    // B2(y) = y^2 - 2y^3 + y^4
    double B2 = y * y - 2.0 * y * y * y + y * y * y * y;
    // D''(x) = -6 + 12x
    double D_xx = -6.0 + 12.0 * x;
    // B2''(y) = 2 - 12y + 12y^2
    double B2_yy = 2.0 - 12.0 * y + 12.0 * y * y;
    // D(x) = x - 3x^2 + 2x^3
    double D = x - 3.0 * x * x + 2.0 * x * x * x;
    double laplace_w = -2.0 * (B2 * D_xx + B2_yy * D);
    return -laplace_w - 1.0;
}
double MainSolver::u_test1(point &p)
{
    double x = p.x, y = p.y;
    double A2 = x * x * (1 - x) * (1 - x);
    double C = y * (1 - y) * (1 - 2 * y);
    return 2 * A2 * C;
}
double MainSolver::w_test1(point &p)
{
    double x = p.x, y = p.y;
    double B2 = y * y * (1 - y) * (1 - y);
    double D = x * (1 - x) * (1 - 2 * x);
    return -2.0 * B2 * D;
}
double MainSolver::p_test1(point &p)
{
    double x = p.x, y = p.y;

    return x * x - y;
}

const double eps = 0.05;

double MainSolver::f1_test2(point &p)
{
    double x = p.x, y = p.y;
    double laplace_u = eps * 2.0 * M_PI * M_PI * M_PI * sin(2.0 * M_PI * y) * (2.0 * cos(2.0 * M_PI * x) - 1.0);
    double p_x = 2.0 * x * exp(x * x - y * y);
    return -laplace_u + p_x;
}
double MainSolver::f2_test2(point &p)
{
    double x = p.x, y = p.y;
    double laplace_w = eps * 2.0 * M_PI * M_PI * M_PI * sin(2.0 * M_PI * x) * (1.0 - 2.0 * cos(2.0 * M_PI * y));
    double p_y = -2.0 * y * exp(x * x - y * y);
    return -laplace_w + p_y;
}
double MainSolver::u_test2(point &p)
{
    double x = p.x, y = p.y;
    return eps * M_PI * sin(M_PI * x) * sin(M_PI * x) * sin(2.0 * M_PI * y);
}
double MainSolver::w_test2(point &p)
{
    double x = p.x, y = p.y;
    return -eps * M_PI * sin(2.0 * M_PI * x) * sin(M_PI * y) * sin(M_PI * y);
}
double MainSolver::p_test2(point &p)
{
    double x = p.x, y = p.y;
    return exp(x * x - y * y);
}

double MainSolver::f1_test(point &p)
{
    double x = p.x, y = p.y;
    return -2 * (y * (1 - y) + x * (1 - x));
}
double MainSolver::f2_test(point &p)
{
    double x = p.x, y = p.y;
    return -13 * M_PI * M_PI * sin(2 * M_PI * x) * sin(3 * M_PI * y);
}
double MainSolver::f3_test(point &p)
{
    double x = p.x, y = p.y;
    return exp(-x) * (2 * M_PI * sin(M_PI * x) * cos(M_PI * y) +
                      sin(M_PI * y) * sin(M_PI * x) * (2 - 4 * x + (y + x * x) * (1 - 2 * M_PI * M_PI)) +
                      2 * M_PI * sin(M_PI * y) * cos(M_PI * x) * (2 * x - y - x * x));
}
double MainSolver::u1_exact(point &p)
{
    double x = p.x, y = p.y;
    return x * (1 - x) * y * (1 - y);
}
double MainSolver::u2_exact(point &p)
{
    double x = p.x, y = p.y;
    return sin(2 * M_PI * x) * sin(3 * M_PI * y);
}
double MainSolver::u3_exact(point &p)
{
    double x = p.x, y = p.y;
    return (y + x * x) * exp(-x) * sin(M_PI * x) * sin(M_PI * y);
}