/**
 * @file shape.cc
 * @author LEMAIRE Alice 0.5, CAPDEVILLE Maud 0.5
 * @version 0.3
 */

#include "shape.h"

#include <cmath>

using namespace std;

void draw_circle(World &monde, const Cairo::RefPtr<Cairo::Context> &cr, int height,
                 int width, double position_x, double position_y, Color couleur,
                 double radius)
{
  monde.on_draw_cercle(cr, height, width, position_x, position_y, couleur, radius);
}

void draw_segments(World &monde, const Cairo::RefPtr<Cairo::Context> &cr, int height,
                   int width, S2d base, S2d extremity, Color couleur, bool first)
{
  monde.on_draw_segment(cr, height, width, base.x, base.y, extremity.x, extremity.y,
                        couleur, first);
}

double angular_deviation_between_segments(const Segment &seg1, const Segment &seg2)
{
  return norm_pi(M_PI - seg2.angle + seg1.angle);
}

double angular_deviation_point_segment(const S2d &point, const Segment &seg)
{
  double angle_futur_effecteur = angle_segment(seg.base, point);
  return norm_pi(angle_futur_effecteur - seg.angle);
}

double norm_pi(double angle)
{
  if (angle < -M_PI)
    angle += 2 * M_PI;
  else if (angle > M_PI)
    angle -= 2 * M_PI;
  return angle;
}

double angle_segment(const S2d &base, const S2d &extremity)
{
  return atan2(extremity.y - base.y, extremity.x - base.x);
}

double distance(const S2d &p1, const S2d &p2)
{
  return sqrt(square(p1.x - p2.x) + square(p1.y - p2.y));
}

int orientation(S2d p, S2d q, S2d r, double epsilon)
{
  double val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
  double dist = val / distance(p, q);
  if (abs(dist) <= epsilon)
    return 0;

  return (dist > 0) ? 1 : 2;
}

bool on_segment(S2d p, S2d q, S2d r, double epsilon)
{
  S2d pq = {q.x - p.x, q.y - p.y};
  S2d pr = {r.x - p.x, r.y - p.y};
  double s = pq.x * pr.x + pq.y * pr.y;
  double norm_pr = distance(p, r);
  double projection = s / norm_pr;

  if (projection >= -epsilon && projection <= (norm_pr + epsilon))
    return true;

  return false;
}

bool in_superposition(const Segment &seg1, const Segment &seg2, double epsilon)
{
  double diff = abs(angular_deviation_between_segments(seg1, seg2));
  if (diff <= epsilon)
    return true;

  return false;
}

bool do_intersect(S2d p1, S2d q1, S2d p2, S2d q2, double epsilon)
{
  int o1 = orientation(p1, q1, p2, epsilon);
  int o2 = orientation(p1, q1, q2, epsilon);
  int o3 = orientation(p2, q2, p1, epsilon);
  int o4 = orientation(p2, q2, q1, epsilon);

  if (o1 && o2 && o3 && o4 && o1 != o2 && o3 != o4)
    return true;

  if ((o1 == 0) && on_segment(p1, p2, q1, epsilon))
    return true;
  if ((o2 == 0) && on_segment(p1, q2, q1, epsilon))
    return true;
  if ((o3 == 0) && on_segment(p2, p1, q2, epsilon))
    return true;
  if ((o4 == 0) && on_segment(p2, q1, q2, epsilon))
    return true;

  return false;
}

double square(double value) { return value * value; }

S2d move_point(const S2d &point, const S2d &destination, double delta_l)
{
  double hypothenus = distance(destination, point);
  double delta_x = (destination.x - point.x) * (delta_l / hypothenus);
  double delta_y = (destination.y - point.y) * (delta_l / hypothenus);

  S2d new_point = {point.x + delta_x, point.y + delta_y};

  return (new_point);
}
