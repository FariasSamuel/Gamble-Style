#ifndef CARTEANNIVERSAIRE_H
#define CARTEANNIVERSAIRE_H

#include "Carte.hpp"
class Plateau; class Joueur;

class CarteAnniversaire : public Carte {
public:
    CarteAnniversaire(Joueur* titulaire, Plateau* plateau, int montant);
    void action() override;
    void setTitulaire(Joueur* j) override { titulaire_ = j; }

private:
    Joueur*  titulaire_;
    Plateau* plateau_;
    int      montant_;
};

#endif // CARTEANNIVERSAIRE_H
