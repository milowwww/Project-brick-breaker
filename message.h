#ifndef MESSAGE_H
#define MESSAGE_H
//
// message.h  : 13 fonctions pour l'affichage des messages d'erreur
//              et une fonction supplémentaire pour indiquer le succès de la lecture
//
// Version 1.0 du 24.02.2025
//

#include <string>

namespace message
{
// score must be >= 0
std::string invalid_score(int score);
// lives must be >= 0
std::string invalid_lives(int score);

// contained in the arena
std::string brick_outside(double x, double y);
// contained in the arena (ignore radius for the lower bound)
std::string ball_outside(double x, double y);
// the arc (part of the circle above y=0) must be contained in the arena
std::string paddle_outside(double x, double y);

// brick_size >= brick_size_min
std::string invalid_brick_size(double size);
// brick_type is in {0, 1, 2}
std::string invalid_brick_type(int type);
// hit_points is in {1, 2, 3, 4, 5, 6, 7}
std::string invalid_hit_points(int hit_points);
// delta_norm must be <= delta_norm_max
std::string invalid_delta(double x, double y);

// no collision of any sort when using a margin of 0 (instead of epsil_zero)
std::string collision_bricks(size_t brick1_index, size_t brick2_index);
std::string collision_paddle_brick(size_t brick_index);
std::string collision_balls(size_t ball1_index, size_t ball2_index);
std::string collision_ball_brick(size_t ball_index, size_t brick_index);
std::string collision_paddle_ball(size_t ball_index);

// Everything went well => file reading and all validation checks
std::string success();

// The game is won
std::string won();
// The game is lost
std::string lost();
} // namespace message

#endif
