/**
 * @file main.cpp
 * @brief Point d'entrée du jeu GambleStyle.
 */
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "Plateau.hpp"
#include "Joueur.hpp"

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    std::cout << "=== GambleStyle ===" << std::endl;
    Plateau plateau;
    int nbJoueurs = 0;
    while (nbJoueurs < 2 || nbJoueurs > 4) {
        std::cout << "Nombre de joueurs (2-4) : ";
        std::cin >> nbJoueurs;
    }
    for (int i = 0; i < nbJoueurs; ++i) {
        std::string nom;
        std::cout << "Nom du joueur " << i + 1 << " : ";
        std::cin >> nom;
        plateau.ajouterJoueur(new Joueur(nom, 1500));
    }
    plateau.ordre();
    plateau.gamelooping();
    Joueur* g = plateau.fin();
    if (g) std::cout << "\n*** Gagnant : " << g->getNom() << " ***" << std::endl;
    return 0;
}
