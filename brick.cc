#include "brick.h"

Brick::Brick(Square square, BrickType type)
    : square(square), type(type)
{
}

Square Brick::get_square() const
{
    return square;
}

BrickType Brick::get_type() const
{
    return type;
}

unsigned Brick::get_hit_points() const
{
    return 0;
}

RainbowBrick::RainbowBrick(Square square, unsigned hit_points)
    : Brick(square, RAINBOW_BRICK), hit_points(hit_points)
{
}

unsigned RainbowBrick::get_hit_points() const
{
    return hit_points;
}

BallBrick::BallBrick(Square square)
    : Brick(square, BALL_BRICK)
{
}

SplitBrick::SplitBrick(Square square)
    : Brick(square, SPLIT_BRICK)
{
}