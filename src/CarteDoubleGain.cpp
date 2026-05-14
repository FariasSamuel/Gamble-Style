/**
 * @file CarteDoubleGain.cpp
 * @brief Implémentation de CarteDoubleGain.
 */
#include "CarteDoubleGain.hpp"
#include "Joueur.hpp"

CarteDoubleGain::CarteDoubleGain(Joueur* joueur)
    : Carte("Double Gain", "Double vos gains au prochain mini-jeu gagne."),
      joueur(joueur) {}

void CarteDoubleGain::action() {
    if (joueur) joueur->donnerCarte(this);
}
