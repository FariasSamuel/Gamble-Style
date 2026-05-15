// CarteDeplacement.cpp
// Déplace le titulaire vers une case fixe (mode legacy / tests)
// ou d'un nombre aléatoire de cases vers l'avant (mode gameplay).

#include "CarteDeplacement.hpp"
#include "Plateau.hpp"
#include "Joueur.hpp"
#include "Case.hpp"
#include <cstdlib>

// Constructeur legacy : crée une case temporaire à l'index absolu indexCase.
// Utilisé par les tests CppUnit pour vérifier le déplacement vers une position précise.
CarteDeplacement::CarteDeplacement(Joueur* titulaire, Plateau* plateau, int indexCase)
    : Carte("Deplacement", "Deplacement vers la case " + std::to_string(indexCase)),
      titulaire_(titulaire), plateau_(plateau),
      caseTarget_(new Case(indexCase))
{}

// Constructeur gameplay : avance le joueur de minSteps à maxSteps cases au hasard.
CarteDeplacement::CarteDeplacement(Plateau* plateau, int minSteps, int maxSteps)
    : Carte("Deplacement", "Avancez de quelques cases !"),
      titulaire_(nullptr), plateau_(plateau),
      caseTarget_(nullptr), minSteps_(minSteps), maxSteps_(maxSteps)
{}

// Détruit la case temporaire créée en mode legacy.
CarteDeplacement::~CarteDeplacement()
{
    delete caseTarget_;  // nullptr-safe
}

// Mode legacy : téléporte vers caseTarget_ (index absolu fixé à la construction).
// Mode gameplay : avance d'un nombre aléatoire de cases dans [minSteps_, maxSteps_].
void CarteDeplacement::action()
{
    if (!titulaire_) return;

    if (caseTarget_) {
        titulaire_->bougerjoueur(caseTarget_);
        return;
    }

    if (!plateau_) return;
    int nb = plateau_->nbCases();
    if (nb <= 0) return;

    int range = maxSteps_ - minSteps_;
    int steps = minSteps_ + (range > 0 ? std::rand() % (range + 1) : 0);

    // Calcule la nouvelle position à partir de la case actuelle (modulo nb)
    int oldIdx = titulaire_->getCaseActuelle()
                 ? titulaire_->getCaseActuelle()->get_num_case() : 0;
    int newIdx = (oldIdx + steps) % nb;

    Case* target = plateau_->getCase(newIdx);
    if (target) titulaire_->bougerjoueur(target);
}
