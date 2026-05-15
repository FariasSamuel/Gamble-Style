#ifndef CARTEDEPLACEMENT_H
#define CARTEDEPLACEMENT_H

#include "Carte.hpp"
class Plateau; class Joueur; class Case;

class CarteDeplacement : public Carte {
public:
    // Legacy constructor used by tests: teleport to absolute case index
    CarteDeplacement(Joueur* titulaire, Plateau* plateau, int indexCase);
    // Gameplay constructor: move forward minSteps..maxSteps cases at random
    CarteDeplacement(Plateau* plateau, int minSteps, int maxSteps);
    ~CarteDeplacement() override;

    void action() override;
    void setTitulaire(Joueur* j) override { titulaire_ = j; }

private:
    Joueur*  titulaire_;
    Plateau* plateau_;
    Case*    caseTarget_; // non-null in legacy mode
    int      minSteps_{0}, maxSteps_{0};
};

#endif // CARTEDEPLACEMENT_H
