/**
 * @file shape.h
 * @author LEMAIRE Alice, CAPDEVILLE Maud
 * @version 0.3
 */

#ifndef SHAPE_H_INCLUDED
#define SHAPE_H_INCLUDED

#include "graphic.h"

const double epsil_zero(0.5);

struct S2d {
  double x;
  double y;
};

struct Segment {
  S2d base;
  double angle;
  double length;
  S2d extremity;
};

void draw_circle(World &monde, const Cairo::RefPtr<Cairo::Context> &cr, int height,
                 int width, double position_x, double position_y, Color couleur,
                 double radius);

void draw_segments(World &monde, const Cairo::RefPtr<Cairo::Context> &cr, int height,
                   int width, S2d base, S2d extremity, Color couleur, bool first);

// prototypes des fonctions
double angular_deviation_between_segments(const Segment &seg1, const Segment &seg2);
double angular_deviation_point_segment(const S2d &point, const Segment &seg);
double norm_pi(double angle);
double angle_segment(const S2d &base, const S2d &extremity);
double distance(const S2d &p1, const S2d &p2);
int orientation(S2d p, S2d q, S2d r, double epsilon);
bool on_segment(S2d p, S2d q, S2d r, double epsilon);
// void use_epsilon(double &epsil, double epsilon);
bool in_superposition(const Segment &seg1, const Segment &seg2, double epsilon);
bool do_intersect(S2d p1, S2d p2, S2d q1, S2d q2, double epsilon);
double square(double value);
S2d move_point(const S2d &point, const S2d &destination, double delta_l);

#endif
