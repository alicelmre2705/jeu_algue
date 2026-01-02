// Performances détection d'algues à manger

// *****************************************
// Lifeform.h
// *****************************************

class Corail : public Lifeform
{
  // ....

private:
  int id;
  Statut_cor statut_cor;
  Dir_rot_cor dir_rot;
  Statut_dev_cor statut_dev;
  std::vector<Segment> segments;
  vector<shared_ptr<Algue>> tab_algues_proches;
};

// *****************************************
// Simulation.cc
// *****************************************

// LECTURE
// Pour chaque corail, remplir son vector tab_algues_proches
// avec les algues situées dans un cercle de rayon l_repro autour de la base de son effecteur

// UPDATE

// Pour chaque création de nouvelle algue :
// parcourir le vector tab_corail_alive pour ajouter cette nouvelle algue si elle se situe dans le rayon de son effecteur

// Pour chaque corail,
// Si tab_algues_proches non vide, on parcourt son vector tab_algues_proches pour voir si une algue est à portée
// Si oui, on la mange et on la retire du vector tab_algues_proches

// Si tab_algues_proches vide, on parcourt le vector tab_algues pour voir si une algue est à portée
// et on en profite pour remplir son vector tab_algues_proches avec les algues situées dans le rayon de son effecteur

// Si le corail crée un nouveau segment pour s'étendre (EXTEND) :
// clear de son vector tab_algues_proches

// Si le corail se reproduit :
// on garde son vector tab_algues_proches intact (car le corail conserve la même base d'effecteur)
// On remplit le vector tab_algues_proches du nouveau corail
