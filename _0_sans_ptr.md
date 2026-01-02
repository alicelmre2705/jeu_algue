// Version sans aucun pointeur

// *****************************************
// Simulation.cc
// *****************************************

// pour CHAQUE mise à jour (simulation.update())
// Pour chaque scavenger de tab_scavengers

// Si statut == MANGE (=> coral_id_target non vide)

// Appel de la méthode mange() sur le corail cible
// Recherche du corail cible dans tab_corail
for (size_t i = 0; i < tab_corail.size(); ++i)
{
  if (tab_corail[i].get_id() == coral_id_target)
  {
    scavenger.mange(tab_corail[i]);
  }
}

// *****************************************
// Lifeform.cc
// *****************************************

int Scavenger::mange(Coral &coral)
{
  int corail_segments_number = coral.get_tab_segments().size();
  // Si la longueur du dernier segment est <= delta_l
  // => suppression du dernier segment
  if (coral.get_tab_segments().back().length <= delta_l)
  {
    coral.get_tab_segments().pop_back();
    corail_segments_number -= 1;
  }
  else
  {
    // Sinon, le scavenger croque un morceau du corail target
    coral.change_effector_length(l_repro - l_seg_interne);
  }

  // Le scavenger se déplace à l'extrémité du dernier segment du corail
  position = coral.get_tab_segments().back().extremite;

  // Le scavenger grossit
  radius += delta_r_sca;

  return corail_segments_number;
}
