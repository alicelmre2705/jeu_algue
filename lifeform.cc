
/**
 * @file lifeform.cc
 * @author LEMAIRE Alice 0.5, CAPDEVILLE Maud 0.5
 * @version 0.3
 */

#include "lifeform.h"

#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "constantes.h"
#include "message.h"
#include "shape.h"

using namespace std;

// creation entités//

// définition de la super-classe
Lifeform::Lifeform() {}

Lifeform::~Lifeform() {}

S2d Lifeform::set_position(S2d &new_position)
{
  position.x = new_position.x;
  position.y = new_position.y;
  return new_position;
}

void Lifeform::increment_age() { ++age; }

bool Lifeform::age_is_invalid() const
{
  if (age < 0 or age == 0)
  {
    cout << message::lifeform_age(age) << endl;
    return true;
  }
  return false;
}

int Lifeform ::get_age() const { return age; }

S2d Lifeform::get_position() { return position; }

bool Lifeform::field_invalid() const
{
  if (position.x < 1 or position.x > max_ - 1 or position.y < 1 or
      position.y > max_ - 1)
  {
    cout << message::lifeform_center_outside(position.x, position.y) << endl;
    return true;
  }
  return false;
}

// définition de la classe dérivée Algue
Algue::Algue(int position_x, int position_y) : Lifeform()
{
  position.x = position_x;
  position.y = position_y;
  age = 1;
}

Algue::Algue(string line) : Lifeform() { decode(line); }

Algue::~Algue() = default;

void Algue::draw(World &monde, const Cairo::RefPtr<Cairo::Context> &cr, int height,
                 int width)
{
  draw_circle(monde, cr, height, width, position.x, position.y, green, (double)r_alg);
}

bool Algue::is_invalid()
{
  if (field_invalid())
  {
    return true;
  }
  if (age_is_invalid())
  {
    return true;
  }

  return false;
}

void Algue::decode(std::string line)
{
  istringstream data(line);
  data >> position.x >> position.y >> age;
}

// définition de la classe dérivée Corail
Corail::Corail(string line) : Lifeform() { decode(line); }

Corail::Corail(Segment seg, int new_id) : Lifeform()
{
  id = new_id;
  age = 1;
  statut_cor = ALIVE;
  dir_rot = INVTRIGO;
  statut_dev = EXTEND;

  position = seg.base;
  segments.push_back(seg);
}

Corail::~Corail() = default;

void Corail::draw(World &monde, const Cairo::RefPtr<Cairo::Context> &cr, int height,
                  int width)
{
  for (int i = 0; i < segments.size(); ++i)
  {
    Color couleur;
    if (statut_cor == ALIVE)
    {
      couleur = blue;
    }
    else
    {
      couleur = black;
    }

    draw_segments(monde, cr, height, width, segments[i].base, segments[i].extremity,
                  couleur, i == 0);
  }
}

bool Corail::is_invalid(vector<unique_ptr<Corail>> const &tab_corail)
{
  double epsilon = 0.;

  if (field_invalid())
  {
    return true;
  }
  if (segment_field_invalid(epsilon))
  {
    return true;
  }
  if (age_is_invalid())
  {
    return true;
  }
  if (coral_id_is_invalid(tab_corail))
  {
    return true;
  }
  if (segments_superposition(epsilon))
  {
    return true;
  }
  if (segments_intersection_intra_corail(epsilon))
  {
    return true;
  }
  if (segments_intersection_inter_corail(tab_corail, epsilon))
  {
    return true;
  }

  return false;
}

unique_ptr<Corail> Corail::reproduction(int new_id)
{
  // Création d'un nouveau corail avec un premier segment
  unique_ptr<Corail> ptr_new_corail = make_unique<Corail>(segments.back(), new_id);

  // Mise à jour du corail parent qui "détache" son dernier segment
  segments.pop_back();
  change_effector_length(l_repro / 2);

  return ptr_new_corail;
}

bool Corail::read_segment_invalid(std::string line)
{
  istringstream data(line);

  Segment seg;
  data >> seg.angle >> seg.length;

  if (seg.length < l_repro - l_seg_interne or seg.length >= l_repro)
  {
    cout << message::segment_length_outside(id, seg.length) << endl;
    return true;
  }

  if (seg.angle < -M_PI or seg.angle > M_PI)
  {
    cout << message::segment_angle_outside(id, seg.angle) << endl;
    return true;
  }

  if (segments.size() == 0)
  {
    seg.base = position;
  }
  else
  {
    seg.base = segments.back().extremity;
  }

  seg.extremity.x = seg.base.x + seg.length * cos(seg.angle);
  seg.extremity.y = seg.base.y + seg.length * sin(seg.angle);

  segments.push_back(seg);

  return false;
}

void Corail::add_segment()
{
  Segment seg;
  seg.extremity = segments.back().extremity;
  seg.angle = segments.back().angle;
  seg.length = l_repro - l_seg_interne;

  // Mise à jour de l'effecteur, avant ajout du nouveau segment effecteur
  seg.base = change_effector_length(l_seg_interne);
  // Ajout du nouveau segment
  segments.push_back(seg);
}

void Corail::erase_last_segment() { segments.pop_back(); }

void Corail::replace_effector(const Segment &seg)
{
  segments.pop_back();
  segments.push_back(seg);
}

bool Corail::collision(double epsilon, vector<unique_ptr<Corail>> const &tab_corail,
                       Segment const &ancien_effecteur)
{
  // avant : on faisait test de superposition
  if (segments.size() > 1)
  {
    if (segments_superposition_when_rotating(ancien_effecteur))
    {
      return true;
    }
  }
  if (segment_field_invalid(epsilon))
  {
    return true;
  }
  if (segments_intersection_intra_corail(epsilon))
  {
    return true;
  }
  if (segment_intersection_inter_corail(id, segments.size() - 1, segments.back(), tab_corail, epsilon))
  {
    return true;
  }

  return false;
}

// fonctions change
void Corail::change_disponibility() { available = !available; }

void Corail::change_length(const S2d &new_position)
{
  segments.back().length = segments.back().length - delta_l;
  segments.back().extremity = new_position;
}

void Corail::change_status_to_dead()
{
  statut_cor = DEAD;
  cout << "Coral id " << id << " CHANGE STATUS: "
       << ((statut_cor == ALIVE) ? "ALIVE" : "DEAD")
       << endl;
}

void Corail::change_status_dev(Statut_dev_cor statut) { statut_dev = statut; }

S2d Corail::change_effector_length(double new_longueur)
{
  segments.back().length = new_longueur;
  segments.back().extremity.x =
      segments.back().base.x + new_longueur * cos(segments.back().angle);
  segments.back().extremity.y =
      segments.back().base.y + new_longueur * sin(segments.back().angle);

  return segments.back().extremity;
}

void Corail::change_direction_rotation()
{
  switch (dir_rot)
  {
  case TRIGO:
    dir_rot = INVTRIGO;
    break;
  case INVTRIGO:
    dir_rot = TRIGO;
    break;
  }
}

// fonctions get
vector<Algue *> Corail::get_algue_with_distance_max_lrepro(vector<Algue> tab_algue)
{
  vector<Algue *> algue_distance_max_lrepro;
  for (int i = 0; i < tab_algue.size(); ++i)
  {
    double distance_algue_coral =
        distance(get_last_segment().base, tab_algue[i].get_position());
    if (distance_algue_coral <= l_repro)
    {
      Algue *alg = &tab_algue[i];
      algue_distance_max_lrepro.push_back(alg);
    }
  }

  return algue_distance_max_lrepro;
}

int Corail::get_id() const { return id; }

bool Corail::get_disponibility() { return available; }

S2d Corail::get_extremity() { return segments.back().extremity; }

Segment Corail::get_last_segment() const { return segments.back(); }

vector<Segment> Corail::get_tab_segments() { return segments; }

Statut_cor Corail::get_status_coral() const { return statut_cor; }

Dir_rot_cor Corail ::get_direction_rotation() const { return dir_rot; }

Statut_dev_cor Corail::get_status_dev() const { return statut_dev; }

void Corail::decode(std::string line)
{
  istringstream data(line);
  int statut_cor_value;
  int dir_rot_value;
  int statut_dev_value;

  data >> position.x >> position.y >> age >> id >> statut_cor_value >> dir_rot_value >>
      statut_dev_value;

  statut_cor = static_cast<Statut_cor>(statut_cor_value);
  dir_rot = static_cast<Dir_rot_cor>(dir_rot_value);
  statut_dev = static_cast<Statut_dev_cor>(statut_dev_value);
}

bool Corail::segment_field_invalid(double epsilon)
{
  for (size_t i = 0; i < segments.size(); ++i)
  {
    if (segments[i].extremity.x <= epsilon or
        segments[i].extremity.x >= (max_ - epsilon) or
        segments[i].extremity.y <= epsilon or
        segments[i].extremity.y >= (max_ - epsilon))
    {
      cout << message::lifeform_computed_outside(id, segments[i].extremity.x,
                                                 segments[i].extremity.y)
           << endl;
      return true;
    }
  }
  return false;
}

bool Corail::coral_id_is_invalid(vector<unique_ptr<Corail>> const &tab_corail)
{
  if (tab_corail.size() == 0)
    return false;

  for (size_t i(0); i < tab_corail.size(); ++i)
  {
    if (tab_corail[i]->id == id)
    {
      cout << message::lifeform_duplicated_id(tab_corail[i]->id) << endl;
      return true;
    }
  }
  return false;
}

// fonction qui vérifie que deux segments consécutifs ne soient pas superposés
bool Corail::segments_superposition(double epsilon) const
{
  for (size_t i = 0; i < segments.size() - 1; ++i)
  {
    if (in_superposition(segments[i], segments[i + 1], epsilon))
    {
      cout << message::segment_superposition(id, i, i + 1) << endl;
      return true;
    }
  }
  return false;
}

// fonction qui vérifie qu'il n'y a pas eu d'intersection avec un des segments lors de
// la rotation
bool Corail::segments_superposition_when_rotating(
    const Segment &ancien_effecteur) const
{
  Segment new_effector = segments.back();

  double difference_new_effector_segment =
      angular_deviation_between_segments(segments[segments.size() - 2], new_effector);

  double difference_old_effector_segment = angular_deviation_between_segments(
      segments[segments.size() - 2], ancien_effecteur);

  if ((abs(difference_new_effector_segment) < delta_rot) &&
      (abs(difference_old_effector_segment) < delta_rot))
  {
    if ((difference_new_effector_segment < 0 && difference_old_effector_segment > 0) ||
        (difference_new_effector_segment > 0 && difference_old_effector_segment < 0))
    {
      cout << message::segment_superposition(id, segments.size() - 2,
                                             segments.size() - 1)
           << endl;
      return true;
    }
  }

  return false;
}

// fonction vérifiant que les segments de différents coraux ne s'intersectent
// en omettant l'intersection d'un corail avec lui même
bool Corail::segments_intersection_inter_corail(
    vector<unique_ptr<Corail>> const &tab_corail, double epsilon) const
{
  for (size_t i = 0; i < segments.size(); ++i)
  {
    if (segment_intersection_inter_corail(id, i, segments[i], tab_corail, epsilon))
    {
      return true;
    }
  }
  return false;
}

bool Corail::segment_intersection_inter_corail(int id, int segment_index, const Segment &segment,
                                               vector<unique_ptr<Corail>> const &tab_corail,
                                               double epsilon) const
{
  for (size_t j = 0; j < tab_corail.size(); ++j)
  {
    if (id == tab_corail[j]->id)
      continue;

    for (size_t k = 0; k < tab_corail[j]->segments.size(); ++k)
    {
      if (do_intersect(segment.base, segment.extremity,
                       tab_corail[j]->segments[k].base,
                       tab_corail[j]->segments[k].extremity, epsilon))
      {
        cout << message::segment_collision(id, segment_index, tab_corail[j]->id, k) << endl;
        return true;
      }
    }
  }
  return false;
}

bool Corail::segments_intersection_intra_corail(double epsilon) const
{
  for (size_t i = 0; i < segments.size(); ++i)
  {
    for (size_t j = 0; j < segments.size(); ++j)
    {
      if (j == i || j == i + 1 || j == i - 1)
        continue;

      if (do_intersect(segments[i].base, segments[i].extremity, segments[j].base,
                       segments[j].extremity, epsilon))
      {
        cout << message::segment_collision(id, i, id, j) << endl;
        return true;
      }
    }
  }
  return false;
}

// définitions de la classe dérivée Scavenger
Scavenger::Scavenger(string line) : Lifeform() { decode(line); }

Scavenger::Scavenger(S2d new_position)
    : Lifeform()
{
  radius = r_sca;
  status_scavenger = LIBRE;
  age = 1;
  position = new_position;
}

Scavenger::~Scavenger() = default;

void Scavenger::draw(World &monde, const Cairo::RefPtr<Cairo::Context> &cr, int height,
                     int width)
{
  draw_circle(monde, cr, height, width, position.x, position.y, red, (double)radius);
}

bool Scavenger::is_invalid(vector<unique_ptr<Corail>> &tab_corail)
{
  if (field_invalid())
  {
    return true;
  }
  if (radius_invalid())
  {
    return true;
  }
  if (id_target_invalid(tab_corail))
  {
    return true;
  }
  if (age_is_invalid())
  {
    return true;
  }

  return false;
}

Scavenger Scavenger::reproduction(const S2d &old_position)
{
  // maj du radius du parent
  radius = r_sca;

  Scavenger new_sca = Scavenger(old_position);

  return new_sca;
}

int Scavenger::eat_coral()
{
  position = move_point(position, coral_target->get_last_segment().base, delta_l);

  radius += delta_r_sca;

  if (coral_target->get_tab_segments().back().length <= delta_l)
  {
    coral_target->erase_last_segment();
  }
  else
  {
    coral_target->change_length(position);
  }
  return coral_target->get_tab_segments().size();
}

void Scavenger::set_coral_target(Corail &corail_le_plus_proche)
{
  coral_id_target = corail_le_plus_proche.get_id();
  coral_target = &corail_le_plus_proche;
}

void Scavenger::reinit_coral_target()
{
  coral_id_target = 0;
  coral_target = nullptr;
}

void Scavenger::move(const Corail &corail_le_plus_proche)
{
  position = move_point(position, corail_le_plus_proche.get_last_segment().extremity,
                        delta_l);
}

void Scavenger::change_status_scavenger()
{
  if (status_scavenger == MANGE)
  {
    status_scavenger = LIBRE;
  }
  else
  {
    status_scavenger = MANGE;
  }
}

Corail *Scavenger::get_coral_target() { return coral_target; }

int Scavenger::get_radius() { return radius; }

Statut_sca Scavenger::get_statut_sca() const { return status_scavenger; }

int Scavenger::get_id_target() { return coral_id_target; }

void Scavenger::decode(std::string line)
{
  istringstream data(line);
  int status_scavenger_value;
  data >> position.x >> position.y >> age >> radius >> status_scavenger_value;
  status_scavenger = static_cast<Statut_sca>(status_scavenger_value);
  if (status_scavenger == MANGE)
    data >> coral_id_target;
}

bool Scavenger::radius_invalid() const
{
  if (radius < r_sca or radius >= r_sca_repro)
  {
    cout << message::scavenger_radius_outside(radius) << endl;
    return true;
  }
  return false;
}

bool Scavenger::id_target_invalid(vector<unique_ptr<Corail>> &tab_corail)
{
  if (status_scavenger == MANGE)
  {
    for (size_t i = 0; i < tab_corail.size(); ++i)
    {
      if (tab_corail[i]->get_id() == coral_id_target)
      {
        coral_target = tab_corail[i].get();
        // ON AVAIT OUBLIE de changer la disponibilité du corail cible lors de la lecture :
        tab_corail[i]->change_disponibility();

        return false;
      }
    }
    cout << message::lifeform_invalid_id(coral_id_target) << endl;
    return true;
  }
  return false;
}
