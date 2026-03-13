#ifndef POINT_H
#define POINT_H
#include <stdio.h>

class edge;
class point;

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

class edge
{
public:
    point *points[4] = {nullptr, nullptr, nullptr, nullptr};
    int id = 0;
    edge() = default;
    edge(point *p0, point *p1, point *p2, point *p3, int id = -1)
    {
        init(p0, p1, p2, p3, id);
    }
    void init(point *p0, point *p1, point *p2, point *p3, int id)
    {
        points[0] = p0;
        points[1] = p1;
        points[2] = p2;
        points[3] = p3;
        this->id = id;
    }
    ~edge()
    {
        points[0] = points[1] = points[2] = points[3] = nullptr;
    }
    bool belongs(point &p)
    {
        if (points[0]->x <= p.x && p.x <= points[1]->x && points[3]->y <= p.y && p.y <= points[0]->y)
        {
            return true;
        }
        return false;
    }
};
#endif