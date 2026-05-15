#include "CarteArgent.hpp"
#include "Joueur.hpp"
#include <cstdlib>

CarteArgent::CarteArgent(Joueur* titulaire, int montant)
    : Carte("Argent", montant >= 0 ? "Gain" : "Amende"), titulaire_(titulaire), montant_(montant)
{
}

CarteArgent::~CarteArgent() = default;

void CarteArgent::action()
{
    if (!titulaire_) return;
    // 50/50 : gain ou perte du montant
    int delta = (std::rand() % 2 == 0) ? montant_ : -montant_;
    titulaire_->setCapital(titulaire_->getCapital() + delta);
}
