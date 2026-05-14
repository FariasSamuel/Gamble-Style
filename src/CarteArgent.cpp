#include "CarteArgent.hpp"
#include "Joueur.hpp"

CarteArgent::CarteArgent(Joueur* titulaire, int montant)
    : Carte("Argent", montant >= 0 ? "Gain" : "Amende"), titulaire_(titulaire), montant_(montant)
{
}

CarteArgent::~CarteArgent() = default;

void CarteArgent::action()
{
    if (!titulaire_) return;
    titulaire_->setCapital(titulaire_->getCapital() + montant_);
}
