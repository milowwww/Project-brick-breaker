#include "game.h"
#include "constants.h"
#include "message.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace
{
bool is_comment_or_empty(std::string const& line)
{
    std::string trimmed = line;
    size_t first = trimmed.find_first_not_of(" \t\r\n");

    if (first == std::string::npos) {
        return true;
    }

    trimmed = trimmed.substr(first);
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

bool strictly_inside_arena(Square const& s)
{
    double half = s.size / 2.0;

    return (s.center.x - half >= 0.0) &&
           (s.center.x + half <= arena_size) &&
           (s.center.y - half >= 0.0) &&
           (s.center.y + half <= arena_size);
}

bool strictly_inside_arena(Ball const& ball)
{
    Circle const& c = ball.shape;

    return (c.center.x - c.radius > 0.0) &&
           (c.center.x + c.radius < arena_size) &&
           (c.center.y + c.radius < arena_size) &&
           (c.center.y > 0.0);
}

bool paddle_inside_arena(Paddle const& paddle)
{
    Circle const& c = paddle.circle;

    if (c.center.y > 0.0) {
        return false;
    }

    if (c.center.y + c.radius <= 0.0) {
        return false;
    }

    double span_sq = c.radius * c.radius - c.center.y * c.center.y;

    if (span_sq <= 0.0) {
        return false;
    }

    double span = std::sqrt(span_sq);

    return (c.center.x - span > 0.0) &&
           (c.center.x + span < arena_size);
}

bool collide_bricks(Brick const& b1, Brick const& b2)
{
    Square s1 = b1.get_square();
    Square s2 = b2.get_square();

    double half1 = s1.size / 2.0;
    double half2 = s2.size / 2.0;

    double dx = std::abs(s1.center.x - s2.center.x);
    double dy = std::abs(s1.center.y - s2.center.y);

    return (dx < half1 + half2) &&
           (dy < half1 + half2);
}

bool collide_ball_ball(Ball const& b1, Ball const& b2)
{
    Point diff{
        b1.shape.center.x - b2.shape.center.x,
        b1.shape.center.y - b2.shape.center.y
    };

    double radius_sum = b1.shape.radius + b2.shape.radius;

    return norm_squared(diff) < radius_sum * radius_sum;
}

bool collide_ball_brick(Ball const& ball, Brick const& brick)
{
    Square square = brick.get_square();
    double half = square.size / 2.0;

    double dx = ball.shape.center.x - square.center.x;
    double dy = ball.shape.center.y - square.center.y;

    double clamped_x = std::max(-half, std::min(dx, half));
    double clamped_y = std::max(-half, std::min(dy, half));

    double closest_x = square.center.x + clamped_x;
    double closest_y = square.center.y + clamped_y;

    double diff_x = ball.shape.center.x - closest_x;
    double diff_y = ball.shape.center.y - closest_y;

    return diff_x * diff_x + diff_y * diff_y <
           ball.shape.radius * ball.shape.radius;
}

bool collide_paddle_ball(Paddle const& paddle, Ball const& ball)
{
    double dx = paddle.circle.center.x - ball.shape.center.x;
    double dy = paddle.circle.center.y - ball.shape.center.y;

    double radius_sum = paddle.circle.radius + ball.shape.radius;

    return dx * dx + dy * dy < radius_sum * radius_sum;
}

bool collide_paddle_brick(Paddle const& paddle, Brick const& brick)
{
    Ball fake_ball{};
    fake_ball.shape.center = paddle.circle.center;
    fake_ball.shape.radius = paddle.circle.radius;

    return collide_ball_brick(fake_ball, brick);
}
}

bool load_game(std::string const& filename, Game& game)
{
    std::vector<std::string> lines = read_useful_lines(filename);

    if (lines.empty()) {
        return false;
    }

    auto fail = [](std::string const& msg) {
        std::cout << msg;
        return false;
    };

    size_t index = 0;

    // score
    {
        std::istringstream iss(lines[index++]);
        int score = 0;
        iss >> score;

        if (score < 0) {
            return fail(message::invalid_score(score));
        }

        game.score = static_cast<unsigned>(score);
    }

    // lives
    {
        std::istringstream iss(lines[index++]);
        int lives = 0;
        iss >> lives;

        if (lives < 0) {
            return fail(message::invalid_lives(lives));
        }

        game.lives = static_cast<unsigned>(lives);
    }

    // paddle
    {
        std::istringstream iss(lines[index++]);

        iss >> game.paddle.circle.center.x
            >> game.paddle.circle.center.y
            >> game.paddle.circle.radius;

        if (!paddle_inside_arena(game.paddle)) {
            return fail(message::paddle_outside(
                game.paddle.circle.center.x,
                game.paddle.circle.center.y));
        }
    }

    // nombre de briques
    unsigned nb_bricks = 0;
    {
        std::istringstream iss(lines[index++]);
        iss >> nb_bricks;
    }

    game.bricks.clear();

    for (unsigned i = 0; i < nb_bricks; ++i)
    {
        Square square{};
        std::istringstream iss(lines[index++]);

        int type = 0;
        iss >> type
            >> square.center.x
            >> square.center.y
            >> square.size;

        if (square.size < brick_size_min) {
            return fail(message::invalid_brick_size(square.size));
        }

        if (type < 0 || type > 2) {
            return fail(message::invalid_brick_type(type));
        }

        if (!strictly_inside_arena(square)) {
            return fail(message::brick_outside(
                square.center.x,
                square.center.y));
        }

        if (type == RAINBOW_BRICK)
        {
            int hp = 0;
            iss >> hp;

            if (hp < 1 || hp > 7) {
                return fail(message::invalid_hit_points(hp));
            }

            game.bricks.push_back(
                std::make_shared<RainbowBrick>(
                    square,
                    static_cast<unsigned>(hp)
                )
            );
        }
        else if (type == BALL_BRICK)
        {
            game.bricks.push_back(std::make_shared<BallBrick>(square));
        }
        else
        {
            game.bricks.push_back(std::make_shared<SplitBrick>(square));
        }
    }

    // nombre de balles
    unsigned nb_balls = 0;
    {
        std::istringstream iss(lines[index++]);
        iss >> nb_balls;
    }

    game.balls.clear();

    for (unsigned i = 0; i < nb_balls; ++i)
    {
        Ball ball{};
        std::istringstream iss(lines[index++]);

        iss >> ball.shape.center.x
            >> ball.shape.center.y
            >> ball.shape.radius
            >> ball.delta.x
            >> ball.delta.y;

        if (!strictly_inside_arena(ball)) {
            return fail(message::ball_outside(
                ball.shape.center.x,
                ball.shape.center.y));
        }

        if (norm(ball.delta) > delta_norm_max) {
            return fail(message::invalid_delta(
                ball.delta.x,
                ball.delta.y));
        }

        game.balls.push_back(ball);
    }

    // collisions briques-briques
    for (size_t i = 0; i < game.bricks.size(); ++i) {
        for (size_t j = i + 1; j < game.bricks.size(); ++j) {
            if (collide_bricks(*game.bricks[i], *game.bricks[j])) {
                return fail(message::collision_bricks(i, j));
            }
        }
    }

    // collisions paddle-briques
    for (size_t i = 0; i < game.bricks.size(); ++i) {
        if (collide_paddle_brick(game.paddle, *game.bricks[i])) {
            return fail(message::collision_paddle_brick(i));
        }
    }

    // collisions balles-balles
    for (size_t i = 0; i < game.balls.size(); ++i) {
        for (size_t j = i + 1; j < game.balls.size(); ++j) {
            if (collide_ball_ball(game.balls[i], game.balls[j])) {
                return fail(message::collision_balls(i, j));
            }
        }
    }

    // collisions balles-briques
    for (size_t i = 0; i < game.balls.size(); ++i) {
        for (size_t j = 0; j < game.bricks.size(); ++j) {
            if (collide_ball_brick(game.balls[i], *game.bricks[j])) {
                return fail(message::collision_ball_brick(i, j));
            }
        }
    }

    // collisions paddle-balles
    for (size_t i = 0; i < game.balls.size(); ++i) {
        if (collide_paddle_ball(game.paddle, game.balls[i])) {
            return fail(message::collision_paddle_ball(i));
        }
    }

    std::cout << message::success();
    return true;
}

void update_game(Game& game)
{
    for (auto& ball : game.balls)
    {
        ball.shape.center.x += ball.delta.x;
        ball.shape.center.y += ball.delta.y;

        if (ball.shape.center.x - ball.shape.radius <= 0.0)
        {
            ball.delta.x = -ball.delta.x;
            ball.shape.center.x = ball.shape.radius;
        }

        if (ball.shape.center.x + ball.shape.radius >= arena_size)
        {
            ball.delta.x = -ball.delta.x;
            ball.shape.center.x = arena_size - ball.shape.radius;
        }

        if (ball.shape.center.y + ball.shape.radius >= arena_size)
        {
            ball.delta.y = -ball.delta.y;
            ball.shape.center.y = arena_size - ball.shape.radius;
        }

        if (ball.shape.center.y - ball.shape.radius <= 0.0)
        {
            game.balls.clear();
            return;
        }
    }
}

bool game_over(Game const& game)
{
    return game.bricks.empty() ||
           (game.balls.empty() && game.lives == 0);
}

bool save_game(std::string const& filename, Game const& game)
{
    std::ofstream file(filename);

    if (!file) {
        return false;
    }

    file << "# score\n";
    file << game.score << "\n\n";

    file << "# lives\n";
    file << game.lives << "\n\n";

    file << "# paddle\n";
    file << game.paddle.circle.center.x << " "
         << game.paddle.circle.center.y << " "
         << game.paddle.circle.radius << "\n\n";

    file << "# bricks\n";
    file << game.bricks.size() << "\n";

    for (auto const& brick : game.bricks)
    {
        Square square = brick->get_square();
        BrickType type = brick->get_type();

        file << static_cast<int>(type) << " "
             << square.center.x << " "
             << square.center.y << " "
             << square.size;

        if (type == RAINBOW_BRICK) {
            file << " " << brick->get_hit_points();
        }

        file << "\n";
    }

    file << "\n# balls\n";
    file << game.balls.size() << "\n";

    for (auto const& ball : game.balls)
    {
        file << ball.shape.center.x << " "
             << ball.shape.center.y << " "
             << ball.shape.radius << " "
             << ball.delta.x << " "
             << ball.delta.y << "\n";
    }

    return true;
}