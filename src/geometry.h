// Fényes Balázs
// homepage: fenyesb.github.io
// repository: github.com/fenyesb/raycast

#ifndef _GEOMETRY_H
#define _GEOMETRY_H

#include <stdbool.h>
#include <stddef.h>

typedef struct{
    int x,y;
} PointI;

typedef struct{
    double x, y;
} Point;

typedef struct{
    Point p1, p2;
    int color;
} Line;

double getDistance(Point p1, Point p2);
double getLength(Line l);
bool get_line_intersection(double p0_x, double p0_y, double p1_x, double p1_y,
    double p2_x, double p2_y, double p3_x, double p3_y, double *i_x, double *i_y);
bool RaySegmentIntersection(Line ray, Line segment, double* dist);
Point getMidpoint(Line l);

#endif // _GEOMETRY_H
