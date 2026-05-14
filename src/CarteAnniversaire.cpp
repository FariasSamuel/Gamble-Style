/**
 * @file CarteAnniversaire.cpp
 * @brief Implémentation de CarteAnniversaire.
 */
#include "CarteAnniversaire.hpp"
#include "Joueur.hpp"
#include "Plateau.hpp"

CarteAnniversaire::CarteAnniversaire(Joueur* titulaire, Plateau* plateau, int montant)
    : Carte("Anniversaire", "Chaque joueur vous verse " + std::to_string(montant) + "."),
      titulaire(titulaire), plateau(plateau), montant(montant) {}

void CarteAnniversaire::action() {
    if (!plateau || !titulaire) return;
    int nb = plateau->getNbJoueursActifs();
    for (int i = 0; i < nb; ++i) {
        Joueur* j = plateau->getJoueur(i);
        if (j && j != titulaire) {
            plateau->transfertargent(j, titulaire, montant);
        }
    }
}
