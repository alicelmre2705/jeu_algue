/**
 * @file gui.h
 * @author LEMAIRE Alice, CAPDEVILLE Maud
 * @version 0.3
 */

#ifndef GUI_H
#define GUI_H

#include <cairomm/context.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/separator.h>
#include <gtkmm/window.h>

#include "graphic.h"
#include "simulation.h"

constexpr unsigned taille_dessin(256);

class Environnement : public Gtk::DrawingArea
{
public:
  Environnement(Simulation &simulation);
  ~Environnement();

  void adjust_frame(int w = 256, int h = 256);

  void on_draw(const Cairo::RefPtr<Cairo::Context> &cr, int h = 256, int w = 256);
  static const Cairo::RefPtr<Cairo::Context> *ptcr;
  World monde;

protected:
  Simulation &simulation;
};

class MyEvent : public Gtk::Window
{
public:
  MyEvent(Simulation &simulation);

  void on_button_clicked_StartOrStop();
  void on_button_clicked_exit();
  void on_button_clicked_save();
  void on_button_clicked_open();
  void on_button_clicked_step();
  bool on_timeout();
  bool on_window_key_pressed(guint keyval, guint, Gdk::ModifierType state);
  void on_file_dialog_response_open(int response_id, Gtk::FileChooserDialog *dialog);
  void on_file_dialog_response_save(int response_id, Gtk::FileChooserDialog *dialog);

  bool keyb_driven_state;
  void set_infos();
  void on_checkbutton_toggled();
  bool checkbutton_seaweed = false;

protected:
  Simulation &simulation;
  Environnement environnement;
  bool timer_added;
  bool disconnect;
  const int timeout_value;

  Gtk::Box m_main_box;
  Gtk::Box m_widgets;
  Gtk::Box m_environment;
  Gtk::Box m_General;
  Gtk::Box m_info;
  Gtk::Box m_maj;
  Gtk::Box m_algues;
  Gtk::Box m_corails;
  Gtk::Box m_charognards;
  Gtk::Button m_Button_exit;
  Gtk::Button m_Button_open;
  Gtk::Button m_Button_save;
  Gtk::Button m_Button_start;
  Gtk::Button m_Button_step;
  Gtk::CheckButton m_checkbutton_seaweed;
  Gtk::Label m_label_general;
  Gtk::Label m_label_info;
  Gtk::Label m_label_maj;
  Gtk::Label m_label_maj_data;
  Gtk::Label m_label_algues;
  Gtk::Label m_label_algues_data;
  Gtk::Label m_label_corails;
  Gtk::Label m_label_corails_data;
  Gtk::Label m_label_scavenger;
  Gtk::Label m_label_scavenger_data;
  Gtk::Separator s1, s2;
};

#endif
