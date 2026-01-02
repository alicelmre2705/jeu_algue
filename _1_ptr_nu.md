// Version avec des pointeurs nus uniquement (pas de unique_ptr)
// AVANTAGE des unique_ptr : si erreur => elle s'affiche lors de la compilation (ça ne compile pas)
// RISQUE avec ptr nu : si erreur => ça compile, mais ça finit en seg fault lors de l'exécution du programme => plus difficile à débugguer

// *****************************************
// Simulation.cc
// *****************************************

// LECTURE

Corail *ptr_corail = new Corail(line);

if (number_segments >= 1)
{
  std::string subline1;
  for (int i = 0; i < number_segments; ++i)
  {
    getline(fichier >> std::ws, subline1);

    if (ptr_corail->read_segment_invalid(subline1))
      return true;
  }
}

if (ptr_corail->is_invalid(tab_corail))
  return true;

tab_corail.push_back(ptr_corail);

void Simulation::update_eating_scavenger(int i)
{
  S2d old_position = tab_scavenger[i].get_position();
  int number_segment_left = tab_scavenger[i].eat_coral();

  if (tab_scavenger[i].get_radius() >= r_sca_repro)
  {
    cout << "Scavenger index " << i << " is reproducting" << endl;
    tab_scavenger.push_back(tab_scavenger[i].reproduction(old_position));
  }

  cout << "Number of segments left for eaten coral: " << number_segment_left << endl;
  if (number_segment_left == 0)
  {
    // delete tab_scavenger[i].get_coral_target();

    for (int j = 0; j < tab_corail.size(); ++j)
    {
      // PAS BESOIN de .get() pour comparer des pointeurs nus
      if (tab_corail[j] == tab_scavenger[i].get_coral_target())
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

// *****************************************
// RQ: on ne peut PAS faire comme ci dessous, car la variable coral est détruite quand on quitte la fonction lecture
// => l'objet corail est détruit, du coup le pointeur stocké dans tab_corail pointe un objet qui n'existe plus

Corail coral = Corail(line);

// ... test si coral est invalide
tab_corail.push_back(&coral);

// *****************************************
// Lifeform.cc
// *****************************************

Corail *Corail::reproduction(int new_id)
{
  // Création d'un nouveau corail avec un premier segment
  Corail *ptr_new_corail = new Corail(segments.back(), new_id);

  // Mise à jour du corail parent qui "détache" son dernier segment
  segments.pop_back();
  change_effector_length(l_repro / 2);

  return ptr_new_corail;
}

// pas besoin d'utiliser .get() pour copier un ptr nu dans un ptr nu
bool Scavenger::id_target_invalid(vector<Corail *> &tab_corail)
{
  if (status_scavenger == 1)
  {
    for (size_t i = 0; i < tab_corail.size(); ++i)
    {
      if (tab_corail[i]->get_id() == coral_id_target)
      {
        coral_target = tab_corail[i];
        tab_corail[i]->change_disponibility();
        return false;
      }
    }
    cout << message::lifeform_invalid_id(coral_id_target) << endl;
    return true;
  }
  return false;
}
