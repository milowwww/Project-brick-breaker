#ifndef BRICK_H
#define BRICK_H

#include "tools.h"

enum BrickType {
    RAINBOW_BRICK = 0,
    BALL_BRICK = 1,
    SPLIT_BRICK = 2
};

struct Brick {
    Square square;
    BrickType type;
    unsigned hit_points;
};

Brick create_brick(double x, double y, double size, BrickType type, unsigned hit_points);
bool is_destroyed(const Brick& b);
void hit_brick(Brick& b);

#endif