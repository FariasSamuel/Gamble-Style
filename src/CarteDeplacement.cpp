/**
 * @file CarteDeplacement.cpp
 * @brief Implémentation de CarteDeplacement.
 * @project GambleStyle — GM4 INSA Rouen Normandie
 */
#include "CarteDeplacement.hpp"
#include "Joueur.hpp"
#include "Plateau.hpp"
#include "Case.hpp"

CarteDeplacement::CarteDeplacement(Joueur* joueur, Plateau* plateau, int caseIndex)
    : Carte("Deplacement", "Deplacez-vous vers la case " + std::to_string(caseIndex) + "."),
      joueur(joueur), plateau(plateau), caseIndex(caseIndex) {}

void CarteDeplacement::action() {
    // Sécurité de base
    if (!joueur) return;

    // 1. Cas normal (En pleine partie avec un vrai Plateau)
    if (plateau) {
        Case* dest = plateau->getCase(caseIndex);
        if (dest) {
            joueur->bougerjoueur(dest); // Le joueur se lie à la vraie case
            return;
        }
    }

    // 2. Cas dégradé (Test unitaire isolé sans plateau)
    // On met simplement à jour l'index du joueur sans le lier à un pointeur de case.
    joueur->setPosition(caseIndex);
}
