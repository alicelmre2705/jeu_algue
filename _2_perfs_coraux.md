// Performances parcours du vector de coraux

// La duplication d'informations stockées en mémoire est toujours risqué
// <= risque d'oublier de mettre toutes les informations à jour
// ex : bool available
// penser à la mettre à jour en même temps qu'on met à jour coral_target et coral_id_target
// = 3 informations à mettre à jour en même temps

// coral_id_target : on est obligé de le garder, car réclamé pour la sauvegarde des données
// en revanche, on peut se passer du bool available, en créant un vector de pointeurs de coraux morts disponibles

// Pour améliorer encore les performances, on peut aussi créer un vector de pointeurs de coraux vivants
// => éviter de raparcourir la totalité de tab_corail pour vérifier si un corail est ALIVE ou DEAD.

// *****************************************
// Simulation.h
// *****************************************
class Simulation
{
  // ...
  //
private:
  int number_update = 0;
  vector<Algue> tab_algue;
  vector<unique_ptr<Corail>> tab_corail;
  vector<Scavenger> tab_scavenger;

  vector<Corail *> tab_corail_alive;
  vector<Corail *> tab_corail_available;
}
//
// *****************************************
// Simulation.cc
// *****************************************

// LECTURE

unique_ptr<Corail>
    ptr_corail = make_unique<Corail>(line);

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

if (tab_corail.back()->get_status() == ALIVE)
{
  // si le corail est vivant => on stocke son adresse dans le vector tab_corail_alive
  tab_corail_alive.push_back(tab_corail.back().get());
}

// lecture des scavengers
Scavenger scavenger(line);

if (scavenger.is_invalid(tab_corail))
{
  return true;
}

tab_scavenger.push_back(scavenger);
++count;

// UPDATE

void Simulation::update_corals()
{
  // On ne met à jour que les coraux ALIVE :
  int tab_corail_size = tab_corail_alive.size();
  for (int i = 0; i < tab_corail_size; ++i)
  {
    if (tab_corail_alive[i]->get_age() >= max_life_cor)
    {
      tab_corail_alive[i]->change_status_to_dead();

      // on stocke son adresse dans le vector tab_corail_available (il n'a pas encore de scavenger sur lui)
      tab_corail_available.push_back(tab_corail_alive[i]);

      // on le retire du vector tab_corail_alive
      swap(tab_corail_alive[i], tab_corail_alive.back());
      tab_corail_alive.pop_back();
    }
    else
    {
      Segment old_effector = tab_corail_alive[i]->get_last_segment();

      if (old_effector.length < l_repro)
      {
        update_coral_effector_inferior_lrepro(*tab_corail_alive[i], old_effector);
      }
      else
      {
        update_coral_effector_superior_lrepro(*tab_corail_alive[i]);
      }
    }
  }
}

// update_scavengers()

void Simulation::find_closest_coral(int i, int &index_closest_coral,
                                    bool &index_initialized,
                                    double &distance_scavenger_closest_effector)
{
  for (int j = 0; j < tab_corail_available.size(); ++j)
  {
    double distance_scavenger_effector =
        distance(tab_scavenger[i].get_position(),
                 tab_corail_available[j]->get_tab_segments().back().extremity);

    if (!index_initialized ||
        distance_scavenger_effector < distance_scavenger_closest_effector)
    {
      index_closest_coral = j;
      distance_scavenger_closest_effector = distance_scavenger_effector;
      index_initialized = true;
    }
  }
}

void Simulation::update_free_scavenger(int i)
{
  int index_closest_coral;
  bool index_initialized = false;
  double distance_scavenger_closest_effector;

  find_closest_coral(i, index_closest_coral, index_initialized,
                     distance_scavenger_closest_effector);

  if (index_initialized)
  {
    if (distance_scavenger_closest_effector <= delta_l)
    {
      tab_scavenger[i].set_position(
          tab_corail_available[index_closest_coral]->get_tab_segments().back().extremity);
      tab_scavenger[i].change_status_scavenger();
      tab_scavenger[i].set_coral_target(*tab_corail_available[index_closest_coral]);

      // tab_corail[index_closest_coral]->change_disponibility();
      swap(tab_corail_available[index_closest_coral], tab_corail_available.back());
      tab_corail_available.pop_back();
    }
    else
    {
      tab_scavenger[i].move(*tab_corail_available[index_closest_coral]);
    }
  }
}

//************************************************************************************************
// Lifeform.cc
//************************************************************************************************

bool Scavenger::id_target_invalid(vector<unique_ptr<Corail>> &tab_corail)
{
  if (status_scavenger == 1)
  {
    for (size_t i = 0; i < tab_corail.size(); ++i)
    {
      if (tab_corail[i]->get_id() == coral_id_target)
      {
        coral_target = tab_corail[i].get();
        tab_corail_available.push_back(tab_corail[i].get());
        return false;
      }
    }
    cout << message::lifeform_invalid_id(coral_id_target) << endl;
    return true;
  }
  return false;
}
