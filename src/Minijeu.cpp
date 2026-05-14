/**
 * @file Minijeu.cpp
 * @brief Implémentation de Minijeu (version aléatoire).
 */
#include "Minijeu.hpp"
#include "Joueur.hpp"
#include <cstdlib>
#include <algorithm>
#include <vector>

Minijeu::Minijeu() {}
const char* Minijeu::getNom() const { return "Minijeu Aleatoire"; }

Joueur* Minijeu::jouer(Joueur* j1, Joueur* j2) { return (std::rand() % 2 == 0) ? j1 : j2; }

Joueur** Minijeu::jouer(Joueur* j1, Joueur* j2, Joueur* j3) {
    std::vector<Joueur*> v = { j1, j2, j3 };
    std::random_shuffle(v.begin(), v.end());
    Joueur** r = new Joueur*[3];
    for(int i=0; i<3; ++i) r[i] = v[i];
    return r;
}

Joueur** Minijeu::jouer(Joueur* j1, Joueur* j2, Joueur* j3, Joueur* j4) {
    std::vector<Joueur*> v = { j1, j2, j3, j4 };
    std::random_shuffle(v.begin(), v.end());
    Joueur** r = new Joueur*[4];
    for(int i=0; i<4; ++i) r[i] = v[i];
    return r;
}
