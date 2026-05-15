#include "CarteDeplacement.hpp"
#include "Plateau.hpp"
#include "Joueur.hpp"
#include "Case.hpp"
#include <cstdlib>

CarteDeplacement::CarteDeplacement(Joueur* titulaire, Plateau* plateau, int indexCase)
    : Carte("Deplacement", "Deplacement"),
      titulaire_(titulaire), plateau_(plateau),
      caseTarget_(new Case(indexCase))
{}

CarteDeplacement::CarteDeplacement(Plateau* plateau, int minSteps, int maxSteps)
    : Carte("Deplacement", "Deplacement"),
      titulaire_(nullptr), plateau_(plateau),
      caseTarget_(nullptr), minSteps_(minSteps), maxSteps_(maxSteps)
{}

CarteDeplacement::~CarteDeplacement()
{
    delete caseTarget_;
}

void CarteDeplacement::action()
{
    if (!titulaire_) return;

    if (caseTarget_) {
        // Legacy mode: teleport to absolute index
        titulaire_->bougerjoueur(caseTarget_);
        return;
    }

    // Random steps mode
    if (!plateau_) return;
    int nb = plateau_->nbCases();
    if (nb <= 0) return;

    int range = maxSteps_ - minSteps_;
    int steps = minSteps_ + (range > 0 ? std::rand() % (range + 1) : 0);

    int oldIdx = titulaire_->getCaseActuelle()
                 ? titulaire_->getCaseActuelle()->get_num_case() : 0;
    int newIdx = (oldIdx + steps) % nb;

    Case* target = plateau_->getCase(newIdx);
    if (target) titulaire_->bougerjoueur(target);
}
