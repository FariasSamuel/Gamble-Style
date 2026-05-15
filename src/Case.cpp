// Case.cpp
// Implémentation de la classe de base Case.
// Les sous-classes surchargent action() pour définir l'effet de la case.

#include "Case.hpp"

Case::Case(int num) : num_case(num), joueur_actif(nullptr) {}

Case::~Case() = default;

int  Case::get_num_case()  const { return num_case; }
int  Case::getIndex()      const { return num_case; }

// No-op : les cases génériques n'ont pas d'effet sur les joueurs.
void Case::action() {}

void    Case::setJoueurActif(Joueur* j) { joueur_actif = j; }
Joueur* Case::getJoueurActif()    const { return joueur_actif; }
