#ifndef CARTEARGENT_H
#define CARTEARGENT_H

#include "Carte.hpp"

class Joueur;

class CarteArgent : public Carte {
public:
    CarteArgent(Joueur* titulaire, int montant);
    ~CarteArgent() override;

    void action() override;
    void setTitulaire(Joueur* j) override { titulaire_ = j; }

private:
    Joueur* titulaire_;
    int montant_;
};

#endif // CARTEARGENT_H
