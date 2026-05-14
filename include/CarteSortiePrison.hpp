#ifndef CARTESORTIEPRISON_H
#define CARTESORTIEPRISON_H

#include "Carte.hpp"
class Joueur;

class CarteSortiePrison : public Carte {
public:
    static const int ID = 1; // placeholder id used in tests
    CarteSortiePrison(Joueur* titulaire);
    void action() override;
};

#endif // CARTESORTIEPRISON_H
