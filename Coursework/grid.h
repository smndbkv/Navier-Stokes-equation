#ifndef GRID_H
#define GRID_H

#include "point.h"
#include "edge.h"

class grid
{
public:
    int nx = 0, ny = 0, n = 0;
    point *points = nullptr;
    edge *edges = nullptr;

    grid() = default;
    ~grid()
    {
        nx = ny = n = 0;
        delete[] points;
        delete[] edges;
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

    void init(int nx, int ny, int s = 0)
    {
        this->nx = nx;
        this->ny = ny;
        this->n = nx * ny;

        points = new point[nx * ny];
        edges = new edge[(nx - 1) * (ny - 1)];
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
};

#endif
