#ifndef CARTESORTIEPRISON_H
#define CARTESORTIEPRISON_H

#include "Carte.hpp"
class Joueur;

class CarteSortiePrison : public Carte {
public:
    static const int ID = 1;
    CarteSortiePrison(Joueur* titulaire);
    void action() override;
    void setTitulaire(Joueur* j) override { titulaire_ = j; }

private:
    Joueur* titulaire_;
};

#endif // CARTESORTIEPRISON_H
