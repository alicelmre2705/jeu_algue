/**
 * @file simulation.cc
 * @author LEMAIRE Alice 0.5, CAPDEVILLE Maud 0.5
 * @version 0.3
 */

#include "simulation.h"

#include <fstream>
#include <random>
#include <sstream>
#include <string>

#include "lifeform.h"
#include "message.h"
#include "shape.h"

using namespace std;

enum read_state
{
  ALGUE,
  CORAIL,
  SCAVENGER
};

uniform_real_distribution<double> distribution(1, 255);
static default_random_engine e;
double p(alg_birth_rate);

Simulation::Simulation() {}

bool Simulation::read_contain_invalid_data(std::string nom_fichier)
{
  e.seed(1);
  destroy_lifeforms();
  std::string line;
  std::ifstream fichier(nom_fichier);
  if (!fichier.is_open())
  {
    return true;
  }

  int number_algue, number_coral, number_scavenger, number_segments, count(0);
  bool count_initialized = false;
  read_state state = ALGUE;

  while (getline(fichier >> std::ws, line))
  {
    if (line[0] == '#')
    {
      continue;
    }
    if (line.empty())
    {
      continue;
    }
    std::istringstream data(line);
    switch (state)
    {
    case ALGUE:
    {
      if (!count_initialized)
      {
        data >> number_algue;
        count_initialized = true;
        if (number_algue == 0)
        {
          state = CORAIL;
          count_initialized = false;
          count = 0;
        }
        break;
      }
      Algue algue(line);

      if (algue.is_invalid())
      {
        return true;
      }

      tab_algue.push_back(algue);
      ++count;

      if (count == number_algue)
      {
        state = CORAIL;
        count_initialized = false;
        count = 0;
      }
      break;
    }
    case CORAIL:
    {
      if (!count_initialized)
      {
        data >> number_coral;
        count_initialized = true;
        if (number_coral == 0)
        {
          state = SCAVENGER;
          count_initialized = false;
          count = 0;
        }
        break;
      }
      data.seekg(-1, ios::end) >> number_segments;

      unique_ptr<Corail> ptr_corail = make_unique<Corail>(line);

      if (number_segments >= 1)
      {
        std::string subline1;
        for (int i = 0; i < number_segments; ++i)
        {
          getline(fichier >> std::ws, subline1);

          if (ptr_corail->read_segment_invalid(subline1))
          {
            return true;
          }
        }
      }

      if (ptr_corail->is_invalid(tab_corail))
      {
        return true;
      }

      tab_corail.push_back(std::move(ptr_corail));
      ++count;

      if (count == number_coral)
      {
        state = SCAVENGER;
        count_initialized = false;
        count = 0;
      }
      break;
    }
    case SCAVENGER:
    {
      if (!count_initialized)
      {
        data >> number_scavenger;
        count_initialized = true;
        if (number_scavenger == 0)
        {
          count_initialized = false;
          count = 0;
        }
        break;
      }

      Scavenger scavenger(line);

      if (scavenger.is_invalid(tab_corail))
      {
        return true;
      }

      tab_scavenger.push_back(scavenger);
      ++count;

      if (count == number_scavenger)
      {
        break;
      }
      break;
    }
    }
  }

  std::cout << message::success();
  return false;
}

void Simulation::save(ofstream &myfile)
{
  myfile.imbue(std::locale("C"));
  myfile << get_number_algue() << endl;
  for (int i(0); i < get_number_algue(); ++i)
  {
    Algue algue = tab_algue[i];
    myfile << "  " << algue.get_position().x << " " << algue.get_position().y << " "
           << algue.get_age() << endl;
  }
  myfile << endl;
  myfile << get_number_coral() << endl;
  for (int i(0); i < get_number_coral(); ++i)
  {
    Corail corail = *tab_corail[i];
    myfile << "  " << corail.get_position().x << " " << corail.get_position().y << " "
           << corail.get_age() << " " << corail.get_id() << " "
           << static_cast<int>(corail.get_status_coral()) << " "
           << static_cast<int>(corail.get_direction_rotation()) << " "
           << static_cast<int>(corail.get_status_dev()) << " "
           << corail.get_tab_segments().size() << endl;

    vector<Segment> corailSegments = corail.get_tab_segments();
    for (int j(0); j < corailSegments.size(); ++j)
    {
      myfile << "    " << corailSegments[j].angle << " " << corailSegments[j].length
             << endl;
    }
  }
  myfile << endl;
  myfile << get_number_scavenger() << endl;
  for (int i(0); i < get_number_scavenger(); ++i)
  {
    Scavenger scavenger = tab_scavenger[i];
    string cible("");
    Statut_sca status_scavenger = scavenger.get_statut_sca();
    if (status_scavenger == 1)
      cible = " " + std::to_string(scavenger.get_id_target());
    myfile << "  " << scavenger.get_position().x << " " << scavenger.get_position().y
           << " " << scavenger.get_age() << " " << scavenger.get_radius() << " "
           << static_cast<int>(status_scavenger) << cible << endl;
  }
  myfile << endl;
}

void Simulation::destroy_lifeforms()
{
  tab_algue.clear();
  tab_corail.clear();
  tab_scavenger.clear();
}

void Simulation ::draw_simulation(World &monde,
                                  const Cairo::RefPtr<Cairo::Context> &cr, int height,
                                  int width)
{
  for (int i = 0; i < get_number_algue(); ++i)
  {
    tab_algue[i].draw(monde, cr, height, width);
  }

  for (int i = 0; i < get_number_coral(); ++i)
  {
    tab_corail[i]->draw(monde, cr, height, width);
  }

  for (int i = 0; i < get_number_scavenger(); ++i)
  {
    tab_scavenger[i].draw(monde, cr, height, width);
  }
}

void Simulation::update(bool checkbutton_seaweed)
{
  increment_number_update();
  cout << "Simulation number: " << get_number_update() << endl;

  update_algues(checkbutton_seaweed);

  update_corals();

  update_scavengers();
}

void Simulation::update_algues(bool checkbutton_seaweed)
{
  for (int i = 0; i < tab_algue.size(); ++i)
  {
    tab_algue[i].increment_age();
    if (tab_algue[i].get_age() >= max_life_alg)
    {
      swap(tab_algue[i], tab_algue.back());
      tab_algue.pop_back();
      cout << "Seaweed index " << i << " is DEAD" << endl;
    }
  }

  if (checkbutton_seaweed)
    create_seaweed();
}

void Simulation::update_corals()
{
  int tab_corail_size = tab_corail.size();
  for (int i = 0; i < tab_corail_size; ++i)
  {
    cout << "Coral index " << i << " has id: " << tab_corail[i]->get_id() << endl;
    cout << "status: "
         << ((tab_corail[i]->get_status_coral() == ALIVE) ? "ALIVE" : "DEAD") << endl;
    cout << ((tab_corail[i]->get_disponibility()) ? "AVAILABLE" : "UNAVAILABLE")
         << endl;

    if (tab_corail[i]->get_status_coral() == ALIVE)
    {
      cout << "status dev: "
           << ((tab_corail[i]->get_status_dev() == EXTEND) ? "EXTEND" : "REPRO")
           << endl;
      cout << "dir rot: "
           << ((tab_corail[i]->get_direction_rotation() == TRIGO) ? "TRIGO" : "INV") << endl;
      cout << "Coral age: " << tab_corail[i]->get_age() << endl;

      tab_corail[i]->increment_age();
      if (tab_corail[i]->get_age() >= max_life_cor)
      {
        tab_corail[i]->change_status_to_dead();
      }
      else
      {
        Segment old_effector = tab_corail[i]->get_last_segment();
        cout << "Coral length effector: " << old_effector.length << endl;

        if (old_effector.length < l_repro)
        {
          update_coral_effector_inferior_lrepro(*tab_corail[i], old_effector);
        }
        else
        {
          update_coral_effector_superior_lrepro(*tab_corail[i]);
        }
      }
    }
  }
}

void Simulation::update_coral_effector_inferior_lrepro(Corail &coral,
                                                       Segment old_effector)
{
  int index_closest_algue;
  bool index_algue_initialized = false;
  double angle_effector_closest_algue;
  find_closest_algue(coral, old_effector, index_closest_algue, index_algue_initialized,
                     angle_effector_closest_algue);

  if (index_algue_initialized)
  {
    cout << "Coral id " << coral.get_id() << " found seaweed index "
         << index_closest_algue << " to eat" << endl;
    coral_eat_algue(coral, old_effector, index_closest_algue,
                    angle_effector_closest_algue, index_closest_algue);
  }
  else
  {
    cout << "Coral id " << coral.get_id() << " is turning" << endl;
    coral_turn(coral, old_effector);
  }
}

void Simulation::find_closest_algue(Corail &coral, Segment old_effector,
                                    int &index_closest_algue,
                                    bool &index_algue_initialized,
                                    double &angle_effector_closest_algue)
{
  for (int j = 0; j < tab_algue.size(); ++j)
  {
    double distance_effector_algue =
        distance(old_effector.base, tab_algue[j].get_position());

    if (distance_effector_algue <= old_effector.length)
    {
      double angle_effector_algue =
          angular_deviation_point_segment(tab_algue[j].get_position(), old_effector);

      if (abs(angle_effector_algue) <= delta_rot)
      {
        if ((coral.get_direction_rotation() == TRIGO && angle_effector_algue >= 0) ||
            (coral.get_direction_rotation() == INVTRIGO && angle_effector_algue < 0))
        {
          if (!index_algue_initialized)
          {
            index_closest_algue = j;
            index_algue_initialized = true;
            angle_effector_closest_algue = angle_effector_algue;
          }
          else
          {
            // ON A OUBLIE d'utiliser les valeurs aboslues pour comparer les ecarts angulaires
            if (abs(angle_effector_algue) < abs(angle_effector_closest_algue))
            {
              index_closest_algue = j;
              angle_effector_closest_algue = angle_effector_algue;
            }
          }
        }
      }
    }
  }
}

void Simulation::coral_eat_algue(Corail &coral, Segment old_effector,
                                 int index_algue_initialized,
                                 double angle_effector_closest_algue,
                                 int index_closest_algue)
{
  Segment futur_effector;
  futur_effector.base = old_effector.base;
  futur_effector.angle = norm_pi(old_effector.angle + angle_effector_closest_algue);
  futur_effector.length = old_effector.length + delta_l;
  futur_effector.extremity.x =
      futur_effector.base.x + futur_effector.length * cos(futur_effector.angle);
  futur_effector.extremity.y =
      futur_effector.base.y + futur_effector.length * sin(futur_effector.angle);

  coral.replace_effector(futur_effector);

  if (coral.collision(epsil_zero, tab_corail, old_effector))
  {
    cout << "Collision detected : remove rotation" << endl;
    coral.replace_effector(old_effector);
    coral.change_direction_rotation();
    cout << "Coral id " << coral.get_id() << " CHANGE DIR ROT: "
         << ((coral.get_direction_rotation() == TRIGO) ? "TRIGO" : "INVTRIGO") << endl;
  }
  else
  {
    cout << "Coral id " << coral.get_id() << " has eaten seaweed index "
         << index_closest_algue << endl;

    swap(tab_algue[index_closest_algue], tab_algue.back());
    tab_algue.pop_back();
  }
}

void Simulation::coral_turn(Corail &coral, Segment old_effector)
{
  Segment futur_effector;

  futur_effector.base = old_effector.base;
  futur_effector.length = old_effector.length;

  if (coral.get_direction_rotation() == TRIGO)
  {
    futur_effector.angle = norm_pi(old_effector.angle + delta_rot);
  }
  else
  {
    futur_effector.angle = norm_pi(old_effector.angle - delta_rot);
  }

  futur_effector.extremity.x =
      futur_effector.base.x + futur_effector.length * cos(futur_effector.angle);
  futur_effector.extremity.y =
      futur_effector.base.y + futur_effector.length * sin(futur_effector.angle);

  coral.replace_effector(futur_effector);
  if (coral.collision(epsil_zero, tab_corail, old_effector))
  {
    cout << "Collision detected : remove rotation" << endl;
    coral.replace_effector(old_effector);
    coral.change_direction_rotation();
    cout << "Coral id " << coral.get_id() << " CHANGE DIR ROT: "
         << ((coral.get_direction_rotation() == TRIGO) ? "TRIGO" : "INVTRIGO") << endl;
  }
}

void Simulation::update_coral_effector_superior_lrepro(Corail &coral)
{
  if (coral.get_status_dev() == REPRO)
  {
    coral.add_segment();

    cout << "Coral id " << coral.get_id() << " is reproducting" << endl;
    int id_new_coral = get_max_coral_id() + 1;

    tab_corail.push_back(coral.reproduction(id_new_coral));

    coral.change_status_dev(EXTEND);
  }
  else // EXTEND
  {
    coral.add_segment();
    cout << "Coral id " << coral.get_id() << " is extending" << endl;
    coral.change_status_dev(REPRO);
  }

  cout << "Coral id " << coral.get_id() << " CHANGE STATUS DEV: "
       << ((coral.get_status_dev() == EXTEND) ? "EXTEND" : "REPRO") << endl;
}

void Simulation::update_scavengers()
{
  int tab_scavenger_size = tab_scavenger.size();
  for (int i = 0; i < tab_scavenger_size; ++i)
  {
    cout << "Scavenger index " << i << " has status: "
         << ((tab_scavenger[i].get_statut_sca() == LIBRE) ? "LIBRE" : "MANGE") << endl;
    update_scavenger_age_and_life(i);
  }
}

void Simulation::update_scavenger_age_and_life(int i)
{
  tab_scavenger[i].increment_age();
  if (tab_scavenger[i].get_age() >= max_life_sca)
  {
    handle_dead_scavenger(i);
  }
  else
  {
    if (tab_scavenger[i].get_statut_sca() == MANGE)
    {
      cout << "Scavenger index " << i << " eats coral with id: "
           << tab_scavenger[i].get_coral_target()->get_id() << endl;
      update_eating_scavenger(i);
    }
    else // LIBRE => on va essayer de le faire bouger vers le corail mort DISPO le plus proche si y'en a un
    {
      cout << "Scavenger index " << i << " is moving" << endl;
      update_free_scavenger(i);
    }
  }
}

void Simulation::handle_dead_scavenger(int i)
{
  if (tab_scavenger[i].get_coral_target())
  {
    tab_scavenger[i].get_coral_target()->change_disponibility();
    cout << "Coral id " << tab_scavenger[i].get_coral_target()->get_id()
         << " is now AVAILABLE" << endl;
  }

  swap(tab_scavenger[i], tab_scavenger.back());
  tab_scavenger.pop_back();
  cout << "Scavenger index " << i << " is DEAD" << endl;
}

void Simulation::update_eating_scavenger(int i)
{
  S2d old_position = tab_scavenger[i].get_position();

  int number_segment_left = tab_scavenger[i].eat_coral();

  if (tab_scavenger[i].get_radius() >= r_sca_repro)
  {
    cout << "Scavenger index " << i << " is reproducting" << endl;
    Scavenger new_sca = tab_scavenger[i].reproduction(old_position);
    tab_scavenger.push_back(new_sca);
  }

  cout << "Number of segments left for eaten coral: " << number_segment_left << endl;
  if (number_segment_left == 0)
  {
    // delete tab_scavenger[i].get_coral_target();

    for (int j = 0; j < tab_corail.size(); ++j)
    {
      if (tab_corail[j].get() == tab_scavenger[i].get_coral_target())
      {
        tab_scavenger[i].reinit_coral_target();
        cout << "Coral id " << tab_corail[j]->get_id() << " DISAPPEARS" << endl;

        swap(tab_corail[j], tab_corail.back());
        tab_corail.pop_back();
      }
    }
    // cout << "ID du CORAL SUPPRIME: " << tab_scavenger[i].get_coral_target()->get_id() << endl;

    tab_scavenger[i].change_status_scavenger();
    cout << "Scavenger index " << i << " is now LIBRE" << endl;
  }
}

void Simulation::update_free_scavenger(int i) // LIBRE
{
  int index_closest_coral;
  bool index_initialized = false;
  double distance_scavenger_closest_effector;

  find_closest_coral(i, index_closest_coral, index_initialized,
                     distance_scavenger_closest_effector);

  if (index_initialized) // il y a un corail mort dispo
  {
    if (distance_scavenger_closest_effector <= delta_l)
    {
      cout << "Scavenger index " << i << " has arrived on corail id "
           << tab_corail[index_closest_coral]->get_id() << endl;

      tab_scavenger[i].set_position(
          tab_corail[index_closest_coral]->get_tab_segments().back().extremity);

      tab_scavenger[i].change_status_scavenger();

      tab_scavenger[i].set_coral_target(*tab_corail[index_closest_coral]);

      tab_corail[index_closest_coral]->change_disponibility();
      cout << "Coral id " << tab_corail[index_closest_coral]->get_id()
           << " is now UNAVAILABLE" << endl;
    }
    else
    {
      tab_scavenger[i].move(*tab_corail[index_closest_coral]);
      cout << "Scavenger index " << i << " has moved towards coral id "
           << tab_corail[index_closest_coral]->get_id() << endl;
    }
  }
}

void Simulation::find_closest_coral(int i, int &index_closest_coral,
                                    bool &index_initialized,
                                    double &distance_scavenger_closest_effector)
{
  for (int j = 0; j < tab_corail.size(); ++j)
  {
    if (tab_corail[j]->get_status_coral() == DEAD &&
        tab_corail[j]->get_disponibility())
    {
      double distance_scavenger_effector =
          distance(tab_scavenger[i].get_position(),
                   tab_corail[j]->get_tab_segments().back().extremity);

      if (!index_initialized ||
          distance_scavenger_effector < distance_scavenger_closest_effector)
      {
        index_closest_coral = j;
        distance_scavenger_closest_effector = distance_scavenger_effector;
        index_initialized = true;
        cout << "distance between scavenger and closest effector: "
             << distance_scavenger_closest_effector << endl;
      }
    }
  }
}

void Simulation ::create_seaweed()
{
  bernoulli_distribution b(p);

  if (b(e))
  {
    double random_between_x = distribution(e);
    double random_between_y = distribution(e);

    Algue algue((int)random_between_x, (int)random_between_y);
    tab_algue.push_back(algue);
  }
}

int Simulation::get_number_update() const { return number_update; }

void Simulation::increment_number_update() { number_update += 1; }

int Simulation::get_number_algue() const { return tab_algue.size(); }

int Simulation::get_number_coral() const { return tab_corail.size(); }

int Simulation::get_number_scavenger() const { return tab_scavenger.size(); }

int Simulation::get_max_coral_id() const
{
  int maxId = 0;
  for (int i(0); i < get_number_coral(); ++i)
  {
    if (tab_corail[i]->get_id() > maxId)
    {
      maxId = tab_corail[i]->get_id();
    }
  }
  return maxId;
}
