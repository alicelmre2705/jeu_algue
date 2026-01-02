/**
 * @file lifeform.h
 * @author LEMAIRE Alice, CAPDEVILLE Maud
 * @version 0.3
 */

#ifndef LIFEFORM_H_INCLUDED
#define LIFEFORM_H_INCLUDED

#include <vector>

#include "shape.h"

enum Statut_cor
{
  DEAD,
  ALIVE
};
enum Statut_dev_cor
{
  EXTEND,
  REPRO
};
enum Dir_rot_cor
{
  TRIGO,
  INVTRIGO
};
enum Statut_sca
{
  LIBRE,
  MANGE
};

using namespace std;

class World;

class Lifeform
{
protected:
  int age;
  S2d position;

public:
  Lifeform();
  ~Lifeform();

  S2d set_position(S2d &new_position);
  void increment_age();
  bool age_is_invalid() const;
  bool field_invalid() const;
  int get_age() const;
  S2d get_position();
};

class Algue : public Lifeform
{
public:
  Algue(int position_x, int position_y);
  Algue(std::string line);
  ~Algue();

  void draw(World &monde, const Cairo::RefPtr<Cairo::Context> &cr, int height,
            int width);
  bool is_invalid();

private:
  void decode(std::string line);
};

class Corail : public Lifeform
{
public:
  Corail(std::string line);
  Corail(Segment seg, int new_id);
  ~Corail();

  void draw(World &monde, const Cairo::RefPtr<Cairo::Context> &cr, int height,
            int width);
  bool is_invalid(vector<unique_ptr<Corail>> const &tab_corail);

  unique_ptr<Corail> reproduction(int new_id);

  bool read_segment_invalid(std::string line);
  void add_segment();
  void erase_last_segment();
  void replace_effector(const Segment &seg);
  bool collision(double epsilon, vector<unique_ptr<Corail>> const &tab_corail,
                 Segment const &effecteur);

  void change_disponibility();
  void change_length(const S2d &new_position);
  void change_status_to_dead();
  void change_status_dev(Statut_dev_cor statut);
  S2d change_effector_length(double new_longueur);
  void change_direction_rotation();

  vector<Algue *> get_algue_with_distance_max_lrepro(vector<Algue> tab_algue);
  int get_id() const;
  bool get_disponibility();
  S2d get_extremity();
  Segment get_last_segment() const;
  vector<Segment> get_tab_segments();
  Statut_cor get_status_coral() const;
  Dir_rot_cor get_direction_rotation() const;
  Statut_dev_cor get_status_dev() const;

private:
  int id;
  Statut_cor statut_cor;
  Dir_rot_cor dir_rot;
  Statut_dev_cor statut_dev;
  std::vector<Segment> segments;
  bool available = true;

  void decode(std::string line);
  bool segment_field_invalid(double epsilon);
  bool coral_id_is_invalid(vector<unique_ptr<Corail>> const &tab_corail);
  bool segments_superposition(double epsilon) const;
  bool segments_superposition_when_rotating(const Segment &old_effector) const;
  bool segments_intersection_inter_corail(vector<unique_ptr<Corail>> const &tab_corail,
                                          double epsilon) const;
  bool segments_intersection_intra_corail(double epsilon) const;
  bool segment_intersection_inter_corail(int id, int segment_index, const Segment &segment,
                                         vector<unique_ptr<Corail>> const &tab_corail,
                                         double epsilon) const;
};

class Scavenger : public Lifeform
{
public:
  Scavenger(std::string line);
  Scavenger(S2d new_position);
  ~Scavenger();

  void draw(World &monde, const Cairo::RefPtr<Cairo::Context> &cr, int height,
            int width);
  bool is_invalid(vector<unique_ptr<Corail>> &tab_corail);

  Scavenger reproduction(const S2d &old_position);
  int eat_coral();

  void set_coral_target(Corail &closest_coral);
  void reinit_coral_target();
  void move(const Corail &closest_coral);
  void change_status_scavenger();

  Corail *get_coral_target();
  int get_radius();
  Statut_sca get_statut_sca() const;
  int get_id_target();

private:
  Statut_sca status_scavenger;
  int radius;
  int coral_id_target;
  Corail *coral_target = nullptr;

  void decode(std::string line);
  bool radius_invalid() const;
  bool id_target_invalid(vector<unique_ptr<Corail>> &tab_corail);
};

#endif
