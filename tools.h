#ifndef TOOLS_H
#define TOOLS_H

constexpr double epsil_zero = 0.125;

struct Point
{
    double x;
    double y;
};

struct Circle
{
    Point center;
    double radius;
};

struct Square
{
    Point center;
    double size;
};

Point operator+(Point const& a, Point const& b);
Point operator-(Point const& a, Point const& b);
Point operator*(double k, Point const& p);

double norm_squared(Point const& p);
double norm(Point const& p);
bool is_zero(double value);

bool intersects(Circle const& c1, Circle const& c2);
bool intersects(Square const& s1, Square const& s2);
bool intersects(Circle const& c, Square const& s);

bool is_inside_arena(Square const& s, double arena_size);
bool is_inside_arena(Circle const& c, double arena_size, bool ignore_bottom_radius);

#endif

