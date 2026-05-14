/**
 * @file CarteAmende.cpp
 * @brief Implémentation de CarteAmende.
 */
#include "CarteAmende.hpp"
#include "Joueur.hpp"
#include "Plateau.hpp"

CarteAmende::CarteAmende(Joueur* joueur, Plateau* plateau, int montant)
    : Carte("Amende", "Payez une amende de " + std::to_string(montant) + "."),
      joueur(joueur), plateau(plateau), montant(montant) {}

void CarteAmende::action() {
    if (!joueur) return;
    if (plateau) plateau->enlevercapital(joueur, montant);
    else         joueur->enleverCapital(montant);
}
