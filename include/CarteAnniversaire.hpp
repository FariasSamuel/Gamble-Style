// CarteAnniversaire.hpp
// Carte "Joyeux Anniversaire" : chaque adversaire verse un montant fixe au
// titulaire de la carte. Le montant est défini dans board.txt (paramètre de
// la case aleatoire correspondante).

#ifndef CARTEANNIVERSAIRE_H
#define CARTEANNIVERSAIRE_H

#include "Carte.hpp"

class Plateau;
class Joueur;

// CarteAnniversaire — effet immédiat : collecte montant_ € auprès de chaque
// autre joueur vivant (RICHE ou BANQUEROUTE) enregistré dans plateau_.
class CarteAnniversaire : public Carte {
public:
    // titulaire : peut être nullptr à la construction (sera mis à jour par setTitulaire).
    // plateau   : nécessaire pour itérer sur tous les joueurs.
    // montant   : somme prélevée par adversaire (€).
    CarteAnniversaire(Joueur* titulaire, Plateau* plateau, int montant);

    // Prélève montant_ € sur chaque adversaire et les verse au titulaire.
    void action() override;

    // Mis à jour par CaseAleatoire::action() avant chaque tirage.
    void setTitulaire(Joueur* j) override { titulaire_ = j; }

private:
    Joueur*  titulaire_;  // bénéficiaire des transferts (non propriétaire)
    Plateau* plateau_;    // source de la liste des joueurs (non propriétaire)
    int      montant_;    // montant prélevé par adversaire (€)
};

#endif // CARTEANNIVERSAIRE_H
