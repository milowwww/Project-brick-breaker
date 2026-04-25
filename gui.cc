#include <filesystem>
#include <iostream>
#include <algorithm>
#include <cmath>
#include "constants.h"
#include "graphic_gui.h"
#include "gui.h"

using namespace std;

enum Response
{
    CANCEL,
    OPEN_FILE,
    SAVE_FILE
};
enum Buttons
{
    EXIT,
    OPEN,
    SAVE,
    RESTART,
    START,
    STEP
};
bool paddle_collides_with_brick(Paddle const& paddle, Brick const& brick)
{
    Square square = brick.get_square();
    double half = square.size / 2.0;

    double dx = paddle.circle.center.x - square.center.x;
    double dy = paddle.circle.center.y - square.center.y;

    double clamped_x = std::max(-half, std::min(dx, half));
    double clamped_y = std::max(-half, std::min(dy, half));

    double closest_x = square.center.x + clamped_x;
    double closest_y = square.center.y + clamped_y;

    double diff_x = paddle.circle.center.x - closest_x;
    double diff_y = paddle.circle.center.y - closest_y;

    return diff_x * diff_x + diff_y * diff_y <
           paddle.circle.radius * paddle.circle.radius;
}
constexpr unsigned drawing_size(500);
void clear_game(Game& game)
{
    game.score = 0;
    game.lives = 0;
    game.bricks.clear();
    game.balls.clear();
}
My_window::My_window(string file_name)
    : main_box(Gtk::Orientation::HORIZONTAL), panel_box(Gtk::Orientation::VERTICAL),
      command_box(Gtk::Orientation::VERTICAL), loop_activated(false),
      buttons({Gtk::Button("exit"), Gtk::Button("open"), Gtk::Button("save"),
               Gtk::Button("restart"), Gtk::Button("start"), Gtk::Button("step")}),
      info_frame("Infos :"), info_text({Gtk::Label("score:"), Gtk::Label("lives:"),
                                        Gtk::Label("bricks:"), Gtk::Label("balls:")})
{
    set_title("Brick Breaker");
    set_child(main_box);
    main_box.append(panel_box);
    main_box.append(drawing);
    panel_box.append(command_box);
    panel_box.append(info_frame);

    set_commands();
    set_key_controller();
    set_mouse_controller();
    set_infos();
    set_drawing();
    if (file_name != "")
{
    Game temp_game;

    if (load_game(file_name, temp_game))
    {
        game = temp_game;
        current_file = file_name;
    }
    else
    {
        clear_game(game);
        current_file = "";
    }
}

update_infos();
}
void My_window::set_commands()
{
    for (auto &button : buttons)
    {
        command_box.append(button);
        command_box.set_size_request(120, -1);
        button.set_margin(1);
    }

    buttons[EXIT].signal_clicked().connect(
        sigc::mem_fun(*this, &My_window::exit_clicked));
    buttons[OPEN].signal_clicked().connect(
        sigc::mem_fun(*this, &My_window::open_clicked));
    buttons[SAVE].signal_clicked().connect(
        sigc::mem_fun(*this, &My_window::save_clicked));
    buttons[RESTART].signal_clicked().connect(
        sigc::mem_fun(*this, &My_window::restart_clicked));
    buttons[START].signal_clicked().connect(
        sigc::mem_fun(*this, &My_window::start_clicked));
    buttons[STEP].signal_clicked().connect(
        sigc::mem_fun(*this, &My_window::step_clicked));
}

void My_window::exit_clicked()
{
    hide();
}
void My_window::open_clicked()
{
    auto dialog = new Gtk::FileChooserDialog("Choose a text file",
                                             Gtk::FileChooserDialog::Action::OPEN);
    set_dialog(dialog);
}
void My_window::save_clicked()
{
    auto dialog = new Gtk::FileChooserDialog("Choose a text file",
                                             Gtk::FileChooserDialog::Action::SAVE);
    set_dialog(dialog);
}
void My_window::restart_clicked()
{
    if (current_file != "")
    {
        Game temp_game;

        if (load_game(current_file, temp_game))
        {
            game = temp_game;
        }
        else
        {
            game = Game{};
            current_file = "";
        }

        update_infos();
        drawing.queue_draw();
    }
}
void My_window::start_clicked()
{
    cout << __func__ << endl;
    if (loop_activated)
    {
        loop_conn.disconnect();
        loop_activated = false;
        buttons[EXIT].set_sensitive(true);
        buttons[OPEN].set_sensitive(true);
        buttons[SAVE].set_sensitive(true);
        buttons[RESTART].set_sensitive(true);
        buttons[START].set_label("start");
        buttons[STEP].set_sensitive(true);
    }
    else if (!game_over(game))
    {
        loop_conn =
            Glib::signal_timeout().connect(sigc::mem_fun(*this, &My_window::loop), dt);
        loop_activated = true;
        buttons[EXIT].set_sensitive(false);
        buttons[OPEN].set_sensitive(false);
        buttons[SAVE].set_sensitive(false);
        buttons[RESTART].set_sensitive(false);
        buttons[START].set_label("stop");
        buttons[STEP].set_sensitive(false);
    }
}
void My_window::step_clicked()
{
    if (!game.balls.empty())
    {
        update_game(game);
        update_infos();
        drawing.queue_draw();
    }
}
void My_window::set_key_controller()
{
    auto contr = Gtk::EventControllerKey::create();
    contr->signal_key_pressed().connect(sigc::mem_fun(*this, &My_window::key_pressed),
                                        false);
    add_controller(contr);
}
bool My_window::key_pressed(guint keyval, guint keycode, Gdk::ModifierType state)
{(void)keycode;
(void)state;
    switch (keyval)
    {
    case '1':
    if (!game.balls.empty())
    {
        update_game(game);
        update_infos();
        drawing.queue_draw();
    }
    return true;
    case 's':
    start_clicked();
    return true;

case 'r':
    restart_clicked();
    return true;
    default:
        break;
    }
    return false;
}

void My_window::set_dialog(Gtk::FileChooserDialog *dialog)
{
    dialog->set_modal(true);
    dialog->set_transient_for(*this);
    dialog->set_select_multiple(false);
    dialog->signal_response().connect(
        sigc::bind(sigc::mem_fun(*this, &My_window::dialog_response), dialog));

    dialog->add_button("_Cancel", CANCEL);
    switch (dialog->get_action())
    {
    case Gtk::FileChooserDialog::Action::OPEN:
        dialog->add_button("_Open", OPEN_FILE);
        break;
    case Gtk::FileChooserDialog::Action::SAVE:
        dialog->add_button("_Save", SAVE_FILE);
        break;
    default:
        break;
    }

    auto filter_text = Gtk::FileFilter::create();
    filter_text->set_name("Text files");
    filter_text->add_pattern("*.txt");
    dialog->add_filter(filter_text);

    auto filter_any = Gtk::FileFilter::create();
    filter_any->set_name("Any files");
    filter_any->add_pattern("*");
    dialog->add_filter(filter_any);

    dialog->show();
}
void My_window::dialog_response(int response, Gtk::FileChooserDialog *dialog)
{
    filesystem::path file_name = "";
    if (dialog->get_file())
    {
        file_name = dialog->get_file()->get_path();
        if (file_name.extension() != ".txt")
        {
            file_name = "";
        }
    }
    switch (response)
    {
    case CANCEL:
        dialog->hide();
        break;
    case OPEN_FILE:
    if (file_name != "")
    {
        Game temp_game;

        if (load_game(file_name.string(), temp_game))
        {
            game = temp_game;
            current_file = file_name.string();
        }
        else
        {
            clear_game(game);
        }

        update_infos();
        drawing.queue_draw();
        dialog->hide();
    }
    break;
    case SAVE_FILE:
    if (file_name != "")
    {
        save_game(file_name.string(), game);
        dialog->hide();
    }
    break;
    default:
        break;
    }
}

bool My_window::loop()
{
    if (loop_activated)
    {
        if (!game.balls.empty())
        {
            update_game(game);
            update_infos();
            drawing.queue_draw();
            return true;
        }
    }
    return false;
}

void My_window::set_infos()
{
    info_frame.set_child(info_grid);
    info_grid.set_column_homogeneous(true);
    for (size_t i(0); i < info_text.size(); ++i)
    {
        info_grid.attach(info_text[i], 0, i, 1, 1);
        info_grid.attach(info_value[i], 1, i, 1, 1);
        info_text[i].set_halign(Gtk::Align::START);
        info_value[i].set_halign(Gtk::Align::END);
        info_text[i].set_margin(3);
        info_value[i].set_margin(3);
    }
}

void My_window::update_infos()
// TODO: update the different counters
{
    info_value[0].set_text(std::to_string(game.score));
info_value[1].set_text(std::to_string(game.lives));
info_value[2].set_text(std::to_string(game.bricks.size()));
info_value[3].set_text(std::to_string(game.balls.size()));
}

void My_window::set_drawing()
{
    drawing.set_content_width(drawing_size);
    drawing.set_content_height(drawing_size);
    drawing.set_expand();
    drawing.set_draw_func(sigc::mem_fun(*this, &My_window::on_draw));
}
void My_window::on_draw(const Cairo::RefPtr<Cairo::Context> &cr, int width, int height)
{
    graphic_set_context(cr);
    double side(min(width, height));
    cr->translate((width - side) / 2, (height + side) / 2);
    cr->scale(side / (arena_size), -side / (arena_size));
    // arène
Square arena;
arena.center = {arena_size / 2.0, arena_size / 2.0};
arena.size = arena_size;
graphic_draw_square(arena, GREY, false);

// briques
for (auto const& brick : game.bricks)
{
    Square square = brick->get_square();
    BrickType type = brick->get_type();
    unsigned hit_points = brick->get_hit_points();

    Color color = RED;

    if (type == RAINBOW_BRICK) {
        switch (hit_points) {
        case 1: color = RED; break;
        case 2: color = ORANGE; break;
        case 3: color = YELLOW; break;
        case 4: color = GREEN; break;
        case 5: color = CYAN; break;
        case 6: color = BLUE; break;
        case 7: color = PURPLE; break;
        default: color = RED; break;
        }
        graphic_draw_square(square, color, true);
    }
    else if (type == BALL_BRICK) {
        graphic_draw_square(square, RED, true);

        Circle c;
        c.center = square.center;
        c.radius = new_ball_radius;
        graphic_draw_circle(c, BLACK, true);
    }
    else if (type == SPLIT_BRICK) {
        graphic_draw_square(square, RED, true);
    }
}

// balles
for (auto const& ball : game.balls)
{
    graphic_draw_circle(ball.shape, BLACK, true);
}

// raquette
graphic_draw_arc(game.paddle.circle, BLACK);
}

void My_window::set_mouse_controller()
{
    auto left_click = Gtk::GestureClick::create();
    auto move = Gtk::EventControllerMotion::create();

    left_click->set_button(GDK_BUTTON_PRIMARY);

    left_click->signal_pressed().connect(
        sigc::mem_fun(*this, &My_window::on_drawing_left_click));
    move->signal_motion().connect(sigc::mem_fun(*this, &My_window::on_drawing_move));

    drawing.add_controller(left_click);
    drawing.add_controller(move);
}
void My_window::on_drawing_left_click(int n_press, double x, double y)
{
    (void)n_press;
(void)x;
(void)y;
    if (game.balls.empty() && game.lives > 0)
    {
        Ball ball;

        ball.shape.radius = new_ball_radius;
        ball.shape.center.x = game.paddle.circle.center.x;
        ball.shape.center.y = game.paddle.circle.center.y + game.paddle.circle.radius
                              + new_ball_radius + epsil_zero;

        ball.delta.x = 0.0;
        ball.delta.y = new_ball_delta_norm;

        game.balls.push_back(ball);
        --game.lives;

        update_infos();
        drawing.queue_draw();
    }
}
void My_window::on_drawing_move(double x, double y)
{
    int width = drawing.get_width();
    int height = drawing.get_height();

    double side = std::min(width, height);

    double model_x = (x - (width - side) / 2.0) * arena_size / side;

    if (model_x < 0.0) {
        model_x = 0.0;
    }

    if (model_x > arena_size) {
        model_x = arena_size;
    }

    double old_x = game.paddle.circle.center.x;
    game.paddle.circle.center.x = model_x;

    // sécurité : si la raquette sort de l’arène, on annule
    double y0 = game.paddle.circle.center.y;
    double r = game.paddle.circle.radius;

    double span_sq = r * r - y0 * y0;

    if (span_sq <= 0.0) {
        game.paddle.circle.center.x = old_x;
        return;
    }

    double span = std::sqrt(span_sq);

    if (game.paddle.circle.center.x - span < 0.0 ||
        game.paddle.circle.center.x + span > arena_size) {
        game.paddle.circle.center.x = old_x;
        return;
    }
for (auto const& brick : game.bricks)
{
    if (paddle_collides_with_brick(game.paddle, *brick))
    {
        game.paddle.circle.center.x = old_x;
        return;
    }
}
    drawing.queue_draw();
}
