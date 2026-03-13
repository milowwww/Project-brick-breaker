// message.cc  : 13 fonctions pour l'affichage des messages d'erreur
//              et une fonction supplémentaire pour indiquer le succès de la lecture
//
//
// Version 1.0 du 24.02.2025
//

#include "message.h"

using namespace std;

// l'espace de nom non-nommé sert ici à restreindre l'usage de la fonction
// reorder_for_consistency à ce module ; c'est une fonction utilitaire locale
namespace // non-nommé
{
void reorder_for_consistency(size_t &index1, size_t &index2)
{
    if (index1 > index2)
    {
        swap(index1, index2);
    }
}
} // namespace

std::string message::invalid_score(int score)
{
    return "Score (" + to_string(score) + ") must be >= 0\n";
}
std::string message::invalid_lives(int lives)
{
    return "Lives (" + to_string(lives) + ") must be >= 0\n";
}

std::string message::brick_outside(double x, double y)
{
    return "Brick at (" + to_string(x) + ";" + to_string(y) +
           ") is is_outside the arena\n";
}
std::string message::ball_outside(double x, double y)
{
    return "Ball at (" + to_string(x) + ";" + to_string(y) +
           ") is is_outside the arena\n";
}
std::string message::paddle_outside(double x, double y)
{
    return "Paddle at (" + to_string(x) + ";" + to_string(y) +
           ") is is_outside its allowed arena\n";
}

std::string message::invalid_brick_size(double size)
{
    return "Brick size (" + to_string(size) + ") must be >= brick_size_min\n";
}
std::string message::invalid_brick_type(int type)
{
    return "Brick type (" + to_string(type) + ") must be in [0, 2]\n";
}
std::string message::invalid_hit_points(int hit_points)
{
    return "Color number (" + to_string(hit_points) + ") must be in [1, 7]\n";
}
std::string message::invalid_delta(double x, double y)
{
    return "Delta norm (" + to_string(x) + ";" + to_string(y) +
           ") must be <= delta_norm_max\n";
}

std::string message::collision_bricks(size_t brick1_index, size_t brick2_index)
{
    reorder_for_consistency(brick1_index, brick2_index);
    return "Brick " + to_string(brick1_index) + " superposed with brick " +
           to_string(brick2_index) + "\n";
}
std::string message::collision_paddle_brick(size_t brick_index)
{
    return "Paddle superposed with brick " + to_string(brick_index) + "\n";
}
std::string message::collision_balls(size_t ball1_index, size_t ball2_index)
{
    reorder_for_consistency(ball1_index, ball2_index);
    return "Ball " + to_string(ball1_index) + " superposed with ball " +
           to_string(ball2_index) + "\n";
}
std::string message::collision_ball_brick(size_t ball_index, size_t brick_index)
{
    return "Ball " + to_string(ball_index) + " superposed with brick " +
           to_string(brick_index) + "\n";
}
std::string message::collision_paddle_ball(size_t ball_index)
{
    return "Paddle superposed with ball " + to_string(ball_index) + "\n";
}

std::string message::success()
{
    return "Correct file\n";
}

std::string message::won()
{
    return "You won!\n";
}

std::string message::lost()
{
    return "Game over\n";
}
