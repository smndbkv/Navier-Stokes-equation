#ifndef POINT_H
#define POINT_H
#include <stdio.h>

class edge;

class point
{
public:
    double x = 0, y = 0;
    int id = 0;
    edge *edges[4] = {nullptr, nullptr, nullptr, nullptr};
    point() = default;
    point(double x, double y, int id = -1)
    {
        init(x, y, id);
    }
    void init(double x, double y, int id = -1)
    {
        this->x = x;
        this->y = y;
        this->id = id;
    }
    void print()
    {
        printf("x = %lf, y = %lf\n", x, y);
    }
};

#endif