#include "game.h"
#include "constants.h"
#include "message.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
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

    double norm_squared_local(Point const& p)
    {
        return p.x * p.x + p.y * p.y;
    }

    double norm_local(Point const& p)
    {
        return std::sqrt(norm_squared_local(p));
    }

    bool strictly_inside_arena(Square const& s)
    {
        double half = s.size / 2.0;
        return (s.center.x - half > 0.0) &&
               (s.center.x + half < arena_size) &&
               (s.center.y - half > 0.0) &&
               (s.center.y + half < arena_size);
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

        double x_span_sq = c.radius * c.radius - c.center.y * c.center.y;
        if (x_span_sq <= 0.0) {
            return false;
        }

        double x_span = std::sqrt(x_span_sq);
        double left = c.center.x - x_span;
        double right = c.center.x + x_span;

        return (left > 0.0) && (right < arena_size);
    }

    bool collide_bricks(Brick const& b1, Brick const& b2)
    {
        double half1 = b1.square.size / 2.0;
        double half2 = b2.square.size / 2.0;

        double dx = std::abs(b1.square.center.x - b2.square.center.x);
        double dy = std::abs(b1.square.center.y - b2.square.center.y);

        return (dx < half1 + half2) && (dy < half1 + half2);
    }

    bool collide_ball_ball(Ball const& b1, Ball const& b2)
    {
        Point diff{b1.shape.center.x - b2.shape.center.x,
                   b1.shape.center.y - b2.shape.center.y};
        double radius_sum = b1.shape.radius + b2.shape.radius;
        return norm_squared_local(diff) < radius_sum * radius_sum;
    }

    bool collide_ball_brick(Ball const& ball, Brick const& brick)
    {
        double half = brick.square.size / 2.0;
        double dx = ball.shape.center.x - brick.square.center.x;
        double dy = ball.shape.center.y - brick.square.center.y;

        double clamped_x = std::max(-half, std::min(dx, half));
        double clamped_y = std::max(-half, std::min(dy, half));

        double closest_x = brick.square.center.x + clamped_x;
        double closest_y = brick.square.center.y + clamped_y;

        double diff_x = ball.shape.center.x - closest_x;
        double diff_y = ball.shape.center.y - closest_y;

        return diff_x * diff_x + diff_y * diff_y < ball.shape.radius * ball.shape.radius;
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
        fake_ball.delta = {0.0, 0.0};
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

    {
        std::istringstream iss(lines[index]);
        int score = 0;
        iss >> score;
        if (score < 0) {
            return fail(message::invalid_score(score));
        }
        game.score = static_cast<unsigned>(score);
        ++index;
    }

    {
        std::istringstream iss(lines[index]);
        int lives = 0;
        iss >> lives;
        if (lives < 0) {
            return fail(message::invalid_lives(lives));
        }
        game.lives = static_cast<unsigned>(lives);
        ++index;
    }

    {
        std::istringstream iss(lines[index]);
        iss >> game.paddle.circle.center.x
            >> game.paddle.circle.center.y
            >> game.paddle.circle.radius;
        if (!paddle_inside_arena(game.paddle)) {
            return fail(message::paddle_outside(game.paddle.circle.center.x,
                                                game.paddle.circle.center.y));
        }
        ++index;
    }

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

        int type_int = 0;
        iss >> type_int
            >> brick.square.center.x
            >> brick.square.center.y
            >> brick.square.size;

        if (brick.square.size < brick_size_min) {
            return fail(message::invalid_brick_size(brick.square.size));
        }

        if (type_int < 0 || type_int > 2) {
            return fail(message::invalid_brick_type(type_int));
        }
        brick.type = static_cast<BrickType>(type_int);

        if (!strictly_inside_arena(brick.square)) {
            return fail(message::brick_outside(brick.square.center.x,
                                               brick.square.center.y));
        }

        if (brick.type == RAINBOW_BRICK) {
            int hit_points = 0;
            iss >> hit_points;
            if (hit_points < 1 || hit_points > 7) {
                return fail(message::invalid_hit_points(hit_points));
            }
            brick.hit_points = static_cast<unsigned>(hit_points);
        } else {
            brick.hit_points = 0;
        }

        game.bricks.push_back(brick);
        ++index;
    }

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

        iss >> ball.shape.center.x
            >> ball.shape.center.y
            >> ball.shape.radius
            >> ball.delta.x
            >> ball.delta.y;

        if (!strictly_inside_arena(ball)) {
            return fail(message::ball_outside(ball.shape.center.x,
                                              ball.shape.center.y));
        }

        if (norm_local(ball.delta) > delta_norm_max) {
            return fail(message::invalid_delta(ball.delta.x, ball.delta.y));
        }

        game.balls.push_back(ball);
        ++index;
    }

    for (size_t i = 0; i < game.bricks.size(); ++i) {
        for (size_t j = i + 1; j < game.bricks.size(); ++j) {
            if (collide_bricks(game.bricks[i], game.bricks[j])) {
                return fail(message::collision_bricks(i, j));
            }
        }
    }

    for (size_t i = 0; i < game.bricks.size(); ++i) {
        if (collide_paddle_brick(game.paddle, game.bricks[i])) {
            return fail(message::collision_paddle_brick(i));
        }
    }

    for (size_t i = 0; i < game.balls.size(); ++i) {
        for (size_t j = i + 1; j < game.balls.size(); ++j) {
            if (collide_ball_ball(game.balls[i], game.balls[j])) {
                return fail(message::collision_balls(i, j));
            }
        }
    }

    for (size_t i = 0; i < game.balls.size(); ++i) {
        for (size_t j = 0; j < game.bricks.size(); ++j) {
            if (collide_ball_brick(game.balls[i], game.bricks[j])) {
                return fail(message::collision_ball_brick(i, j));
            }
        }
    }

    for (size_t i = 0; i < game.balls.size(); ++i) {
        if (collide_paddle_ball(game.paddle, game.balls[i])) {
            return fail(message::collision_paddle_ball(i));
        }
    }

    std::cout << message::success();
    return true;
}