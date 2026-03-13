#include "tools.h"

#include <cmath>
#include <algorithm>

Point operator+(Point const& a, Point const& b)
{
    return {a.x + b.x, a.y + b.y};
}

Point operator-(Point const& a, Point const& b)
{
    return {a.x - b.x, a.y - b.y};
}

Point operator*(double k, Point const& p)
{
    return {k * p.x, k * p.y};
}

double norm_squared(Point const& p)
{
    return p.x * p.x + p.y * p.y;
}

double norm(Point const& p)
{
    return std::sqrt(norm_squared(p));
}

bool is_zero(double value)
{
    return std::abs(value) < epsil_zero;
}

bool intersects(Circle const& c1, Circle const& c2)
{
    Point diff = c1.center - c2.center;
    double distance_sq = norm_squared(diff);
    double radius_sum = c1.radius + c2.radius;

    return distance_sq < radius_sum * radius_sum - epsil_zero;
}

bool intersects(Square const& s1, Square const& s2)
{
    double half1 = s1.side / 2.0;
    double half2 = s2.side / 2.0;

    double dx = std::abs(s1.center.x - s2.center.x);
    double dy = std::abs(s1.center.y - s2.center.y);

    return (dx < half1 + half2 - epsil_zero) &&
           (dy < half1 + half2 - epsil_zero);
}

bool intersects(Circle const& c, Square const& s)
{
    double half = s.side / 2.0;

    double dx = c.center.x - s.center.x;
    double dy = c.center.y - s.center.y;

    double clamped_x = std::max(-half, std::min(dx, half));
    double clamped_y = std::max(-half, std::min(dy, half));

    Point closest = {s.center.x + clamped_x, s.center.y + clamped_y};

    Point diff = c.center - closest;
    double distance_sq = norm_squared(diff);

    return distance_sq < c.radius * c.radius - epsil_zero;
}

bool is_inside_arena(Square const& s, double arena_size)
{
    double half = s.side / 2.0;

    double left   = s.center.x - half;
    double right  = s.center.x + half;
    double bottom = s.center.y - half;
    double top    = s.center.y + half;

    return (left   > epsil_zero) &&
           (right  < arena_size - epsil_zero) &&
           (bottom > epsil_zero) &&
           (top    < arena_size - epsil_zero);
}

bool is_inside_arena(Circle const& c, double arena_size, bool ignore_bottom_radius)
{
    bool left_ok  = (c.center.x - c.radius > epsil_zero);
    bool right_ok = (c.center.x + c.radius < arena_size - epsil_zero);
    bool top_ok   = (c.center.y + c.radius < arena_size - epsil_zero);

    bool bottom_ok;
    if (ignore_bottom_radius) {
        bottom_ok = (c.center.y > epsil_zero);
    } else {
        bottom_ok = (c.center.y - c.radius > epsil_zero);
    }

    return left_ok && right_ok && top_ok && bottom_ok;
}

