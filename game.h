#ifndef GAME_H
#define GAME_H

#include <string>
#include <vector>

#include "ball.h"
#include "brick.h"
#include "paddle.h"

struct Game
{
    unsigned score;
    unsigned lives;
    Paddle paddle;
    std::vector<Brick> bricks;
    std::vector<Ball> balls;
};

bool load_game(std::string const& filename, Game& game);

#endif
