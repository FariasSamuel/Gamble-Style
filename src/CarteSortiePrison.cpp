/**
 * @file CarteSortiePrison.cpp
 * @brief Implémentation de CarteSortiePrison.
 */
#include "CarteSortiePrison.hpp"
#include "Joueur.hpp"

CarteSortiePrison::CarteSortiePrison(Joueur* joueur)
    : Carte("Sortie Prison", "Sortez de prison sans payer."),
      joueur(joueur) {}

void CarteSortiePrison::action() {
    if (joueur && !joueur->possedeCarte(ID))
        joueur->donnerCarte(this);
}
