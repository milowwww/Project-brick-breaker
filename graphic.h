#ifndef GRAPHIC_H
#define GRAPHIC_H

#include "tools.h"

enum Color
{
    RED,
    ORANGE,
    YELLOW,
    GREEN,
    CYAN,
    BLUE,
    PURPLE,
    BLACK,
    GREY,
    WHITE,
};

void graphic_draw_square(Square const& square, Color color, bool filled);
void graphic_draw_circle(Circle const& circle, Color color, bool filled);
void graphic_draw_arc(Circle const& circle, Color color);

#endif