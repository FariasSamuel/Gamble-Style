// CarteArgent.hpp
// Carte "Bonne ou Mauvaise Nouvelle" : tirage aléatoire 50/50.
// Le titulaire gagne montant_ € (pile) ou perd montant_ € (face).

#ifndef CARTEARGENT_H
#define CARTEARGENT_H

#include "Carte.hpp"

class Joueur;

// CarteArgent — effet immédiat aléatoire : +montant_ ou -montant_ sur le capital.
class CarteArgent : public Carte {
public:
    // titulaire : peut être nullptr (mis à jour par setTitulaire avant le tirage).
    // montant   : valeur absolue du gain ou de la perte (€).
    CarteArgent(Joueur* titulaire, int montant);
    ~CarteArgent() override;

    // Modifie le capital du titulaire : +montant_ (50 % de chance) ou -montant_.
    void action() override;

    // Mis à jour par CaseAleatoire::action() avant chaque tirage.
    void setTitulaire(Joueur* j) override { titulaire_ = j; }

private:
    Joueur* titulaire_;  // joueur concerné (non propriétaire)
    int     montant_;    // valeur absolue du changement de capital (€)
};

#endif // CARTEARGENT_H
