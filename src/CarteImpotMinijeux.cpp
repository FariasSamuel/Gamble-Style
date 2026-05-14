/**
 * @file CarteImpotMinijeux.cpp
 * @brief Implémentation de CarteImpotMinijeux.
 */
#include "CarteImpotMinijeux.hpp"
#include "Joueur.hpp"
#include "Plateau.hpp"

CarteImpotMinijeux::CarteImpotMinijeux(Joueur* joueur, Plateau* plateau, int taux)
    : Carte("Impot Minijeux", "Payez " + std::to_string(taux) + " par case possedee."),
      joueur(joueur), plateau(plateau), tauxParCase(taux) {}

void CarteImpotMinijeux::action() {
    if (!joueur) return;
    int nb = joueur->getNbProprietes();
    if (nb == 0) return;
    int impot = tauxParCase * nb;
    if (plateau) plateau->enlevercapital(joueur, impot);
    else         joueur->enleverCapital(impot);
}
