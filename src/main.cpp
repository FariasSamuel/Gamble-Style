// main.cpp
// Point d'entrée du jeu GambleStyle.
// Crée le plateau SFML, lance l'écran de configuration, puis la boucle de jeu.
// Tout le cycle de vie (joueurs, cases, fenêtre) est géré par PlateauSFML.

#include "PlateauSFML.hpp"

int main()
{
    PlateauSFML plateau;
    plateau.initialization();  // écran de configuration (nb joueurs, noms, durée)
    plateau.gamelooping();     // boucle SFML principale jusqu'à fermeture de la fenêtre
    return 0;
}
