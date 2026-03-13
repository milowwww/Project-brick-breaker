#ifndef GAME_H
#define GAME_H

#include <vector>

#include "ball.h"
#include "brick.h"
#include "paddle.h"

struct Game
{
    int score;
    int lives;
    Paddle paddle;
    std::vector<Brick> bricks;
    std::vector<Ball> balls;
};

#endif
