#include <stdio.h>
#include <fstream>
#include <time.h>
#include "MainSolver.h"
#include "point.h"

int main(int argc, char **argv)
{
    int nx, ny;
    double t1, t2, t3;
    if (!(argc == 3 && sscanf(argv[1], "%d", &nx) == 1 && sscanf(argv[2], "%d", &ny) == 1))
    {
        printf("Usage: %s nx ny\n", argv[0]);
        return 0;
    }
    MainSolver Solver(nx, ny, false);
    // Solver.init_D_y();
    //  Solver.draw_grid_velocity();
    //  Solver.draw_grid_pressure();

    // point p = {0.5, 0.5};
    // // G.draw_point(p);
    // edge ed = Solver.linear_search(p);
    // // G.draw_edge(ed);
    // //  printf("%d\n", ed.id);
    // // int k = ed.points[0]->id;
    // // int k = 6;
    // // G.draw_phi(k);
    // // G.draw_phi_x(k);
    // // G.draw_f2();
    // // G.solve_linear_system_f();
    // // G.draw_f_approximate();
    // // printf("redisual_f = %10.3e\n", G.get_r_f());

    // Solver.draw_psi(1);

    // // Solver.draw_u_exact();
    t1 = clock();
    Solver.init_matr();
    // Solver.print_matr();
    bool st = Solver.is_symmetric(1e-15);
    if (st == false)
    {
        printf("Matrix is not symmetric!\n");
    }
    st = Solver.is_skew_symmetric(1e-15);
    if (st == false)
    {
        printf("Matrix is not skew-symmetric!\n");
    }
    // Solver.print_matr();
    t1 = (clock() - t1) / CLOCKS_PER_SEC;

    t2 = clock();
    Solver.solve_linear_system();
    t2 = (clock() - t2) / CLOCKS_PER_SEC;
    // // Solver.draw_u_approximate();
    // Solver.print_x();

    t3 = clock();
    double r_u_c = Solver.get_residual_c(&MainSolver::u_exact, &MainSolver::u_approximate);
    double r_w_c = Solver.get_residual_c(&MainSolver::w_exact, &MainSolver::w_approximate);
    double r_p_c = Solver.get_residual_c(&MainSolver::p_exact, &MainSolver::p_approximate);
    double r_u_l1 = Solver.get_residual_l1(&MainSolver::u_exact, &MainSolver::u_approximate);
    double r_w_l1 = Solver.get_residual_l1(&MainSolver::w_exact, &MainSolver::w_approximate);
    double r_p_l1 = Solver.get_residual_l1(&MainSolver::p_exact, &MainSolver::p_approximate);
    t3 = (clock() - t3) / CLOCKS_PER_SEC;

    printf("C[0,1]^2 : r_u = %10.3e, r_w = %10.3e, r_p = %10.3e,\nL_1[0,1]^2 : r_u = %10.3e, r_w = %10.3e, r_p = %10.3e,\nt1 = %lf, t2 = %lf, t3 = %lf\n",
           r_u_c, r_w_c, r_p_c, r_u_l1, r_w_l1, r_p_l1, t1, t2, t3);

    Solver.visualizeFlow(&MainSolver::u_exact, &MainSolver::w_exact, &MainSolver::p_exact);
    Solver.visualizeFlow(&MainSolver::u_approximate, &MainSolver::w_approximate, &MainSolver::p_approximate);

    Solver.visualizeFlow(&MainSolver::err_u, &MainSolver::err_w, &MainSolver::err_p);

    // G.draw_u2();
    // t1 = clock();
    // G.init_matr();
    // // G.print_A();
    // t1 = (clock() - t1) / CLOCKS_PER_SEC;

    // t2 = clock();
    // G.solve_linear_system();
    // t2 = (clock() - t2) / CLOCKS_PER_SEC;
    // G.draw_u_approximate();

    // t3 = clock();
    // double r = G.get_r_u();
    // t3 = (clock() - t3) / CLOCKS_PER_SEC;

    // printf("redisual_u = %10.3e, t1 = %lf, t2 = %lf, t3 = %lf\n", r, t1, t2, t3);
    //   printf("%lf\n", G.phi(p, k));
    //   G.plot3d(0, 1, 100, 0, 1, 100);
    //   G.integral_phi_square(0);
    //   G.integral_phi(k);
    //   G.draw_point_edges(k / nx, k % nx);
    //   G.init_matr_m();
    // G.print_matr();
    return 0;
}
