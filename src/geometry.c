// Fényes Balázs
// homepage: fenyesb.github.io
// repository: github.com/fenyesb/raycast

#include <math.h>
#include <stdbool.h>

#include "geometry.h"

//egy szakasz hossza
double getLength(Line l)
{
    return getDistance(l.p1,l.p2);
}

//két pont távolsága
double getDistance(Point p1, Point p2)
{
    return sqrt(pow(p1.x-p2.x,2)+pow(p1.y-p2.y,2));
}

//forrás: http://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect/1968345#1968345
// Returns 1 if the lines intersect, otherwise 0. In addition, if the lines
// intersect the intersection point may be stored in the doubles i_x and i_y.
bool get_line_intersection(double p0_x, double p0_y, double p1_x, double p1_y,
    double p2_x, double p2_y, double p3_x, double p3_y, double *i_x, double *i_y)
{
    double s1_x, s1_y, s2_x, s2_y;
    s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
    s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

    double s, t;
    s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0 &&s<=1  && t >= 0 && t <= 1)
    {
        *i_x = p0_x + (t * s1_x);
        *i_y = p0_y + (t * s1_y);
        return true;
    }

    return false; // No collision
}

//metszi-e egymást egy sugár és egy szakasz, ha igen, akkor milyen távol
bool RaySegmentIntersection(Line ray, Line segment, double* dist)
{
    double i_x, i_y;
    if(!get_line_intersection(ray.p1.x,ray.p1.y,ray.p2.x,ray.p2.y,segment.p1.x,segment.p1.y,segment.p2.x,segment.p2.y,&i_x,&i_y))
        return false;
    *dist = getDistance(ray.p1,(Point){i_x,i_y});
    return true;
}

//szakasz felezőpontja
Point getMidpoint(Line l)
{
    return (Point){(l.p1.x+l.p2.x)/2,(l.p1.y+l.p2.y)/2};
}
