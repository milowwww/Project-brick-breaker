#include "game.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace
{
    bool is_comment_or_empty(std::string const& line)
    {
        std::string trimmed = line;

        // enlever les espaces au début
        size_t first = trimmed.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) {
            return true;
        }

        trimmed = trimmed.substr(first);

        // ligne vide ou commentaire
        return trimmed.empty() || trimmed[0] == '#';
    }

    std::vector<std::string> read_useful_lines(std::string const& filename)
    {
        std::vector<std::string> lines;
        std::ifstream file(filename);

        if (!file) {
            return lines;
        }

        std::string line;
        while (std::getline(file, line)) {
            if (!is_comment_or_empty(line)) {
                lines.push_back(line);
            }
        }

        return lines;
    }
}

bool load_game(std::string const& filename, Game& game)
{
    std::vector<std::string> lines = read_useful_lines(filename);

    if (lines.empty()) {
        return false;
    }

    size_t index = 0;

    // score
    {
        std::istringstream iss(lines[index]);
        iss >> game.score;
        ++index;
    }

    // lives
    {
        std::istringstream iss(lines[index]);
        iss >> game.lives;
        ++index;
    }

    // paddle : x y r
    iss >> game.paddle.circle.center.x
    >> game.paddle.circle.center.y
    >> game.paddle.circle.radius;

    // nombre de briques
    unsigned nb_bricks = 0;
    {
        std::istringstream iss(lines[index]);
        iss >> nb_bricks;
        ++index;
    }

    game.bricks.clear();
    for (unsigned i = 0; i < nb_bricks; ++i) {
        Brick brick{};
        std::istringstream iss(lines[index]);

        iss >> brick.square.center.x
        >> brick.square.center.y
        >> brick.square.size;

        // si rainbow brick, on lit aussi hit_points
        if (brick.type == 0) {
            iss >> brick.hit_points;
        } else {
            brick.hit_points = 0;
        }

        game.bricks.push_back(brick);
        ++index;
    }

    // nombre de balles
    unsigned nb_balls = 0;
    {
        std::istringstream iss(lines[index]);
        iss >> nb_balls;
        ++index;
    }

    game.balls.clear();
    for (unsigned i = 0; i < nb_balls; ++i) {
        Ball ball{};
        std::istringstream iss(lines[index]);

        iss >> ball.circle.center.x
            >> ball.circle.center.y
            >> ball.circle.radius
            >> ball.delta.x
            >> ball.delta.y;

        game.balls.push_back(ball);
        ++index;
    }

    return true;
}
