#include "graphic.h"
#include "graphic_gui.h"

using namespace std;

static const Cairo::RefPtr<Cairo::Context> *ptcr(nullptr);

// graphic_gui.h
void graphic_set_context(const Cairo::RefPtr<Cairo::Context> &cr)
{
    ptcr = &cr;
}
// local functions
void set_color(Color color)
{
    double r(0.), g(0.), b(0.);

    switch (color)
    {
    case RED:
        r = 1.0;
        break;
    case ORANGE:
        r = 1.0;
        g = 0.5;
        break;
    case YELLOW:
        r = 1.0;
        g = 1.0;
        break;
    case GREEN:
        g = 1.0;
        break;
    case CYAN:
        g = 1.0;
        b = 1.0;
        break;
    case BLUE:
        b = 1.0;
        break;
    case PURPLE:
        r = 0.5;
        b = 1.0;
        break;
    case BLACK:
        r = g = b = 0.0;
        break;
    case GREY:
        r = g = b = 0.5;
        break;
    case WHITE:
        r = g = b = 1.0;
        break;
    default:
        break;
    }
    (*ptcr)->set_source_rgb(r, g, b);
}
#include <cmath>

void graphic_draw_square(Square const& square, Color color, bool filled)
{
    set_color(color);

    double half = square.size / 2.0;

    (*ptcr)->rectangle(square.center.x - half,
                       square.center.y - half,
                       square.size,
                       square.size);

    if (filled) {
        (*ptcr)->fill();
    } else {
        (*ptcr)->set_line_width(0.5);
        (*ptcr)->stroke();
    }
}

void graphic_draw_circle(Circle const& circle, Color color, bool filled)
{
    set_color(color);

    (*ptcr)->arc(circle.center.x,
                 circle.center.y,
                 circle.radius,
                 0.0,
                 2.0 * M_PI);

    if (filled) {
        (*ptcr)->fill();
    } else {
        (*ptcr)->set_line_width(0.5);
        (*ptcr)->stroke();
    }
}

void graphic_draw_arc(Circle const& circle, Color color)
{
    set_color(color);

    double value = -circle.center.y / circle.radius;

    if (value < -1.0) value = -1.0;
    if (value > 1.0) value = 1.0;

    double angle1 = std::asin(value);
    double angle2 = M_PI - angle1;

    (*ptcr)->set_line_width(0.7);
    (*ptcr)->arc(circle.center.x,
                 circle.center.y,
                 circle.radius,
                 angle1,
                 angle2);
    (*ptcr)->stroke();
}