#ifndef CARTESORTIEPRISON_H
#define CARTESORTIEPRISON_H

#include "Carte.hpp"
class Joueur;

class CarteSortiePrison : public Carte {
public:
    static const int ID = 1;
    CarteSortiePrison(Joueur* titulaire);
    void action() override;

private:
    Joueur* titulaire_;
};

#endif // CARTESORTIEPRISON_H
