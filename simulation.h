/**
 * @file simulation.h
 * @author LEMAIRE Alice, CAPDEVILLE Maud
 * @version 0.3
 */

#ifndef SIMULATION_H
#define SIMULATION_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "constantes.h"
#include "lifeform.h"

using namespace std;

int random_between_y(int min = 1, int max = 255);
int random_between_x(int min = 1, int max = 255);

class Simulation
{
public:
  Simulation();

  bool read_contain_invalid_data(std::string nom_fichier);
  void save(ofstream &myfile);
  void destroy_lifeforms();
  void draw_simulation(World &monde, const Cairo::RefPtr<Cairo::Context> &cr,
                       int height, int width);
  void update(bool checkbutton_seaweed);

  void update_algues(bool checkbutton_seaweed);

  void update_corals();
  void update_coral_effector_inferior_lrepro(Corail &coral, Segment old_effector);
  void find_closest_algue(Corail &coral, Segment old_effector,
                          int &index_closest_algue, bool &index_algue_initialized,
                          double &angle_effector_clostest_algue);
  void coral_eat_algue(Corail &coral, Segment old_effector,
                       int index_algue_initialized,
                       double angle_effector_clostest_algue, int index_closest_algue);
  void coral_turn(Corail &coral, Segment old_effector);
  void update_coral_effector_superior_lrepro(Corail &coral);

  void update_scavengers();
  void update_scavenger_age_and_life(int i);
  void handle_dead_scavenger(int i);
  void update_eating_scavenger(int i);
  void update_free_scavenger(int i);
  void find_closest_coral(int i, int &index_closest_coral,
                          bool &index_initialized,
                          double &distance_scavenger_closest_effector);

  void create_seaweed();

  int get_number_update() const;
  void increment_number_update();

  int get_number_algue() const;
  int get_number_coral() const;
  int get_number_scavenger() const;
  int get_max_coral_id() const;

private:
  int number_update = 0;

  vector<Algue> tab_algue;
  vector<unique_ptr<Corail>> tab_corail;
  vector<Scavenger> tab_scavenger;
};

#endif
