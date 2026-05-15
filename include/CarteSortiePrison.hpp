// CarteSortiePrison.hpp
// Carte conservée en main permettant de sortir de prison immédiatement,
// sans avoir besoin de lancer un double. La carte est retirée de l'inventaire
// après utilisation par PlateauSFML (Joueur::retirerCarte).

#ifndef CARTESORTIEPRISON_H
#define CARTESORTIEPRISON_H

#include "Carte.hpp"

class Joueur;

// CarteSortiePrison — carte d'inventaire à usage unique.
// ID statique utilisé par Joueur::possedeCarte() pour détecter sa présence.
class CarteSortiePrison : public Carte {
public:
    static const int ID = 1;  // identifiant unique pour possedeCarte()

    // titulaire : joueur qui recevra la carte (peut être nullptr si mis à jour par setTitulaire).
    explicit CarteSortiePrison(Joueur* titulaire);

    // Place la carte dans l'inventaire du titulaire via Joueur::donnerCarte().
    void action() override;

    // Mis à jour par CaseAleatoire::action() avant chaque tirage.
    void setTitulaire(Joueur* j) override { titulaire_ = j; }

private:
    Joueur* titulaire_;  // destinataire de la carte (non propriétaire)
};

#endif // CARTESORTIEPRISON_H
