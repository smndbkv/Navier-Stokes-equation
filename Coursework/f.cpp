#include "MainSolver.h"

double MainSolver::f1(point &p) // правая часть
{
    return f1_test1(p);
}
double MainSolver::f2(point &p) // правая часть
{
    return f2_test1(p);
}

double MainSolver::u_exact(point &p)
{
    return u_test1(p);
}
double MainSolver::w_exact(point &p)
{
    return w_test1(p);
}
double MainSolver::p_exact(point &p)
{
    return p_test1(p);
}

double MainSolver::u0(point &p) // значения на границе
{
    (void)p;
    return 0;
}

double MainSolver::f1_test1(point &p)
{
    double x = p.x, y = p.y;
    return (1 - 2 * x) * y * (1 - y);
}
double MainSolver::f2_test1(point &p)
{
    double x = p.x, y = p.y;
    return (1 - 2 * y) * x * (1 - x);
}
double MainSolver::u_test1(point &p)
{
    (void)p;
    return 0;
}
double MainSolver::w_test1(point &p)
{
    (void)p;
    return 0;
}
double MainSolver::p_test1(point &p)
{
    double x = p.x, y = p.y;

    return x * (1 - x) * y * (1 - y);
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