/**
 * @file graphic.cc
 * @author LEMAIRE Alice 0.8, CAPDEVILLE Maud 0.2
 * @version 0.3
 */

#include "graphic.h"

#include <cairomm/context.h>
#include <gtkmm/label.h>

#include <iostream>
#include <string>

#include "gui.h"

World::World() {}

World::~World() {}

void World::on_draw_limite(const Cairo::RefPtr<Cairo::Context> &cr, int h, int w) {
  cr->set_line_width(1.);
  cr->set_source_rgb(light_grey.r, light_grey.g, light_grey.b);
  cr->rectangle(0, 0, max_, max_);
  cr->stroke();
}

void World::on_draw_cercle(const Cairo::RefPtr<Cairo::Context> &cr, int width,
                           int height, double position_x, double position_y,
                           Color couleur, double radius) {
  cr->set_line_width(1.);
  cr->set_source_rgb(couleur.r, couleur.g, couleur.b);
  cr->arc(position_x, position_y, radius, 0., 2. * M_PI);
  cr->set_line_width(1.);
  cr->stroke();
}

void World::on_draw_segment(const Cairo::RefPtr<Cairo::Context> &cr, int width,
                            int height, double position_x, double position_y,
                            double extremite_x, double extremite_y, Color couleur,
                            bool first) {
  cr->set_source_rgb(couleur.r, couleur.g, couleur.b);
  cr->set_line_width(1.);
  cr->move_to(position_x, position_y);
  cr->line_to(extremite_x, extremite_y);
  if (first) cr->rectangle(position_x, position_y, 4, 4);
  cr->stroke();
}

void World::on_draw_world(const Cairo::RefPtr<Cairo::Context> &cr, int width,
                          int height) {
  adjust_frame(width, height);
  orthographic_projection(cr, frame);
}

void World::adjust_frame(int width, int height) {
  frame.width = width;
  frame.height = height;

  double new_aspect_ratio((double) width / height);
  if (new_aspect_ratio > default_frame.aspect_ratio) {
    frame.y_max = default_frame.y_max;
    frame.y_min = default_frame.y_min;

    double delta(default_frame.x_max - default_frame.x_min);
    double mid((default_frame.x_max + default_frame.x_min) / 2);

    frame.x_max = mid + 0.5 * (new_aspect_ratio / default_frame.aspect_ratio) * delta;
    frame.x_min = mid - 0.5 * (new_aspect_ratio / default_frame.aspect_ratio) * delta;
  } else {
    frame.x_max = default_frame.x_max;
    frame.x_min = default_frame.x_min;

    double delta(default_frame.y_max - default_frame.y_min);
    double mid((default_frame.y_max + default_frame.y_min) / 2);

    frame.y_max = mid + 0.5 * (default_frame.aspect_ratio / new_aspect_ratio) * delta;
    frame.y_min = mid - 0.5 * (default_frame.aspect_ratio / new_aspect_ratio) * delta;
  }
}

void World::orthographic_projection(const Cairo::RefPtr<Cairo::Context> &cr,
                                    Outline frame) {
  double midX = (frame.x_min + frame.x_max) / 2;
  double midY = (frame.y_min + frame.y_max) / 2;
  double scale_factor = 1.3;

  cr->translate(frame.width / 2, frame.height / 2);
  cr->scale(frame.width / (scale_factor * (frame.x_max - frame.x_min)),
            -frame.height / (scale_factor * (frame.y_max - frame.y_min)));

  cr->translate(-midX, -midY);
  cr->translate(-max_ / 2, -max_ / 2);
}
