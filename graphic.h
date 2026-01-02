/**
 * @file graphic.h
 * @author LEMAIRE Alice, CAPDEVILLE Maud
 * @version 0.3
 */

#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <gtkmm.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/window.h>

#include <iostream>
#include <vector>

using namespace std;

constexpr int area_side(200);

struct Color {
  double r;
  double g;
  double b;
};

struct Outline {
  double x_min;
  double x_max;
  double y_min;
  double y_max;
  double aspect_ratio;
  int height;
  int width;
};

static Outline default_frame = {-150., 150., -100., 100., 1.5, 300, 200};

constexpr Color black{0., 0., 0.};
constexpr Color red{1, 0., 0.};
constexpr Color green{0., 1., 0.};
constexpr Color blue{0, 0., 1.};
constexpr Color light_grey{0.8, 0.8, 0.8};

class World : public Gtk::DrawingArea {
public:
  World();
  virtual ~World();

  void on_draw_limite(const Cairo::RefPtr<Cairo::Context> &cr, int w, int h);

  void on_draw_cercle(const Cairo::RefPtr<Cairo::Context> &cr, int width, int height,
                      double position_x, double position_y, Color couleur,
                      double radius);
  void on_draw_segment(const Cairo::RefPtr<Cairo::Context> &cr, int height, int width,
                       double position_x, double position_y, double extremite_x,
                       double extremite_y, Color couleur, bool first);

  void on_draw_world(const Cairo::RefPtr<Cairo::Context> &cr, int width, int height);
  void adjust_frame(int width, int height);
  void orthographic_projection(const Cairo::RefPtr<Cairo::Context> &cr, Outline frame);

  Outline frame;
};

#endif
