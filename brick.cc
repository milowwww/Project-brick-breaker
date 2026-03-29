#include "brick.h"

Brick create_brick(double x, double y, double size, BrickType type, unsigned hit_points)
{
    Brick b;
    b.square.center.x = x;
    b.square.center.y = y;
    b.square.size = size;
    b.type = type;
    b.hit_points = hit_points;
    return b;
}

bool is_destroyed(const Brick& b)
{
    if (b.type == RAINBOW_BRICK) {
        return b.hit_points == 0;
    }
    return false;
}

void hit_brick(Brick& b)
{
    if (b.type == RAINBOW_BRICK && b.hit_points > 0) {
        --b.hit_points;
    }
}