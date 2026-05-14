/**
 * @file Case.cpp
 * @brief Implémentation de la classe de base Case.
 */
#include "Case.hpp"

Case::Case(const std::string& nom, int index)
    : nom(nom), index(index), joueur_actif(nullptr) {}

Case::Case(int index)
    : nom(""), index(index), joueur_actif(nullptr) {}

const std::string& Case::getNom()  const { return nom; }
int Case::getIndex()               const { return index; }
void Case::setJoueurActif(Joueur* j)     { joueur_actif = j; }
Joueur* Case::getJoueurActif()     const { return joueur_actif; }
