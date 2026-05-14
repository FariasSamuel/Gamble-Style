#ifndef CARTEDEPLACEMENT_H
#define CARTEDEPLACEMENT_H

#include "Carte.hpp"
class Plateau; class Joueur;

class CarteDeplacement : public Carte {
public:
    CarteDeplacement(Joueur* titulaire, Plateau* plateau, int indexCase);
    void action() override;
};

#endif // CARTEDEPLACEMENT_H
