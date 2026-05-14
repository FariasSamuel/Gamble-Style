#ifndef CARTEANNIVERSAIRE_H
#define CARTEANNIVERSAIRE_H

#include "Carte.hpp"
class Plateau; class Joueur;

class CarteAnniversaire : public Carte {
public:
    CarteAnniversaire(Joueur* titulaire, Plateau* plateau, int montant);
    void action() override;
};

#endif // CARTEANNIVERSAIRE_H
