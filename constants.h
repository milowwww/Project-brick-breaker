#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "tools.h" // for epsil_zero

constexpr double arena_size = 100.0;
constexpr double new_ball_radius = 1.;
constexpr double new_ball_delta_norm = 0.8;
constexpr double delta_norm_max = 3.;
constexpr double split_brick_gap = 3.;
constexpr double brick_size_min = 3.;
constexpr unsigned nb_bounce_max = 5; // to avoid infinite computation for a stuck ball
constexpr unsigned score_per_hit = 10;
constexpr unsigned score_per_life = 300;
constexpr unsigned dt = 25; // time interval between each step in milliseconds

#endif