#ifndef GAME_H
#define GAME_H
#include <memory>
#include <string>
#include <vector>

#include "ball.h"
#include "brick.h"
#include "paddle.h"

struct Game
{
    unsigned score = 0;
    unsigned lives = 0;
    Paddle paddle{};
    std::vector<std::shared_ptr<Brick>> bricks;
    std::vector<Ball> balls;
};

bool load_game(std::string const& filename, Game& game);
bool save_game(std::string const& filename, Game const& game);
void update_game(Game& game);
bool game_over(Game const& game);

#endif