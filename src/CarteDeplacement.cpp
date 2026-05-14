#include "CarteDeplacement.hpp"
#include "Joueur.hpp"
#include "Case.hpp"

CarteDeplacement::CarteDeplacement(Joueur* titulaire, Plateau* /*plateau*/, int indexCase)
    : Carte("Deplacement", "Deplacement"),
      titulaire_(titulaire),
      caseTarget_(new Case(indexCase))
{
}

CarteDeplacement::~CarteDeplacement()
{
    delete caseTarget_;
}

void CarteDeplacement::action()
{
    if (titulaire_) titulaire_->bougerjoueur(caseTarget_);
}
