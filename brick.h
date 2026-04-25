#ifndef BRICK_H
#define BRICK_H

#include "tools.h"

enum BrickType {
    RAINBOW_BRICK = 0,
    BALL_BRICK = 1,
    SPLIT_BRICK = 2
};

class Brick
{
public:
    Brick(Square square, BrickType type);
    virtual ~Brick() = default;

    Square get_square() const;
    BrickType get_type() const;

    virtual unsigned get_hit_points() const;

protected:
    Square square;
    BrickType type;
};

class RainbowBrick : public Brick
{
public:
    RainbowBrick(Square square, unsigned hit_points);
    unsigned get_hit_points() const override;

private:
    unsigned hit_points;
};

class BallBrick : public Brick
{
public:
    BallBrick(Square square);
};

class SplitBrick : public Brick
{
public:
    SplitBrick(Square square);
};

#endif