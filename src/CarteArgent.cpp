// CarteArgent.cpp
// Tirage aléatoire 50/50 : +montant_ ou -montant_ sur le capital du titulaire.

#include "CarteArgent.hpp"
#include "Joueur.hpp"
#include <cstdlib>

CarteArgent::CarteArgent(Joueur* titulaire, int montant)
    : Carte("Argent", "Bonne ou mauvaise nouvelle ?"),
      titulaire_(titulaire), montant_(montant)
{
}

CarteArgent::~CarteArgent() = default;

// Applique +montant_ (gain) ou -montant_ (perte) avec une probabilité égale.
void CarteArgent::action()
{
    if (!titulaire_) return;
    int delta = (std::rand() % 2 == 0) ? montant_ : -montant_;
    titulaire_->setCapital(titulaire_->getCapital() + delta);
}
