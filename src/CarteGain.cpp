/**
 * @file CarteGain.cpp
 * @brief Implémentation de CarteGain.
 */
#include "CarteGain.hpp"
#include "Joueur.hpp"
#include "Plateau.hpp"

CarteGain::CarteGain(Joueur* joueur, Plateau* plateau, int montant)
    : Carte("Gain", "Vous gagnez " + std::to_string(montant) + "."),
      joueur(joueur), plateau(plateau), montant(montant) {}

void CarteGain::action() {
    if (!joueur) return;
    if (plateau) plateau->donnercapital(joueur, montant);
    else         joueur->donnerCapital(montant);
}
