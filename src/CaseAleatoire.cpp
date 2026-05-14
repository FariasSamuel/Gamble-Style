/**
 * @file CaseAleatoire.cpp
 * @brief Implémentation de CaseAleatoire.
 */
#include "CaseAleatoire.hpp"
#include "Carte.hpp"

CaseAleatoire::CaseAleatoire() : Case("Aleatoire", -1) {}

CaseAleatoire::~CaseAleatoire() {
    for (auto* carte : liste_cartes) { 
        delete carte;
    }
    liste_cartes.clear();
}

void CaseAleatoire::ajouterCarte(Carte* c) {
    if (c) liste_cartes.push_back(c);
}

Carte* CaseAleatoire::getPremiereCarteDeFile() const {
    return liste_cartes.empty() ? nullptr : liste_cartes.front();
}

void CaseAleatoire::action() {
    if (liste_cartes.empty()) return;
    Carte* carte = liste_cartes.front();
    liste_cartes.erase(liste_cartes.begin());
    carte->action();
    liste_cartes.push_back(carte);
}


