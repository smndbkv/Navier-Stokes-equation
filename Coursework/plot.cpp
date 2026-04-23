#include "MainSolver.h"
void MainSolver::draw_grid_velocity()
{
    for (int i = 0; i < ny - 1; i++)
    {
        for (int j = 0; j < nx - 1; j++)
        {
            draw_edge(edges, nx, i, j, 0.7, 0);
        }
    }
}
void MainSolver::draw_grid_pressure()
{
    for (int i = 0; i < ny_pressure - 1; i++)
    {
        for (int j = 0; j < nx_pressure - 1; j++)
        {
            draw_edge(grid_pressure.edges, nx_pressure, i, j, 2, 2);
        }
    }
}
void MainSolver::draw_edge(edge *edges, int nx, int i, int j, double weight, double color)
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
void MainSolver::draw_edge(edge &ed, double weight, double color)
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
void MainSolver::draw_point(point &p)
{
    fprintf(gp, "$data%d << EOD\n", data_i);
    fprintf(gp, "%lf %lf\n", p.x, p.y);
    fprintf(gp, "EOD\n");
    fprintf(gp, "replot $data%d with points pt 7 ps 1 lc 'red' notitle\n", data_i);
    data_i++;
}
void MainSolver::draw_point_edges(int i, int j)
{
    edge **eds = points[i * nx + j].edges;
    for (int r = 0; r < 4; r++)
    {
        if (eds[r] != nullptr)
            draw_edge(*eds[r], 3, 0);
    }
}
void MainSolver::plot3d(double (MainSolver::*f)(point &p, int k, int l), int k, int l, double xmin, double xmax, int nx, double ymin, double ymax, int ny, const char *title)
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
void MainSolver::plot3d(double (MainSolver::*f)(point &p), double xmin, double xmax, int nx, double ymin, double ymax, int ny, const char *title)
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
            double z = (this->*f)(p);
            fprintf(gp3d, "%f %f %f\n", x, y, z);
        }
        fprintf(gp3d, "\n"); // разделитель блоков
    }

    fprintf(gp3d, "e\n"); // конец данных
    fflush(gp3d);         // принудительная отправка

    pclose(gp3d); // закрытие pipe
}
void MainSolver::draw_phi(int k)
{
    plot3d(&MainSolver::phi, k, k, 0, 1, 60, 0, 1, 60);
}
void MainSolver::draw_phi_x(int k)
{
    plot3d(&MainSolver::phi_x, k, k, 0, 1, 60, 0, 1, 60);
}
void MainSolver::draw_u_exact()
{
    plot3d(&MainSolver::u_exact, 0, 1, 60, 0, 1, 60, "u exact");
}
void MainSolver::draw_u_approximate()
{
    plot3d(&MainSolver::u_approximate, 0, 1, 60, 0, 1, 60, "u approximate");
}
void MainSolver::draw_psi(int k)
{
    plot3d(&MainSolver::psi, k, k, 0, 1, 60, 0, 1, 60);
}