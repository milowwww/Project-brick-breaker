#ifndef GUI_H
#define GUI_H

#include <array>
#include <gtkmm.h>
#include <string>
#include "game.h"


class My_window : public Gtk::Window
{
public:
    My_window(std::string file_name);

private:
    Gtk::Box main_box, panel_box, command_box;
    Gtk::Grid info_grid;
    bool loop_activated;
    sigc::connection loop_conn;
    Game game;
std::string current_file;
    std::array<Gtk::Button, 6> buttons;
    Gtk::Frame info_frame;
    std::array<Gtk::Label, 4> info_text, info_value;
    Gtk::DrawingArea drawing;
    

    void set_commands();

    void exit_clicked();
    void open_clicked();
    void save_clicked();
    void restart_clicked();
    void start_clicked();
    void step_clicked();

    void set_key_controller();
    bool key_pressed(guint keyval, guint keycode, Gdk::ModifierType state);

    void set_dialog(Gtk::FileChooserDialog *dialog);
    void dialog_response(int response, Gtk::FileChooserDialog *dialog);

    bool loop();

    void set_infos();
    void update_infos();

    void set_drawing();
    void on_draw(const Cairo::RefPtr<Cairo::Context> &cr, int width, int height);

    void set_mouse_controller();
    void on_drawing_left_click(int n_press, double x, double y);
    void on_drawing_move(double x, double y);
};

#endif