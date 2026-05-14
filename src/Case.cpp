#include "Case.hpp"

Case::Case(int num) : num_case(num) {}

Case::~Case() = default;

int Case::get_num_case() const { return num_case; }

void Case::action() { /* no-op base */ }

void Case::setJoueurActif(Joueur* j) { joueur_actif = j; }

Joueur* Case::getJoueurActif() const { return joueur_actif; }

int Case::getIndex() const { return num_case; }
