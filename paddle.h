#ifndef PADDLE_H
#define PADDLE_H

#include "tools.h"

// Structure représentant la raquette
struct Paddle {
    Circle circle;   // cercle représentant la raquette
};

// Création d'une raquette
Paddle create_paddle(double x, double y, double r);

#endif