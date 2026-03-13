#include "paddle.h"

// Création d'une raquette
Paddle create_paddle(double x, double y, double r)
{
    Paddle p;

    p.circle.center.x = x;
    p.circle.center.y = y;
    p.circle.radius   = r;

    return p;
}
