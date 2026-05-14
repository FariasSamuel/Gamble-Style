/**
 * @file CasePrison.cpp
 * @brief Implémentation de CasePrison.
 */
#include "CasePrison.hpp"
#include "Joueur.hpp"
#include "CarteSortiePrison.hpp" // <-- AJOUT IMPORTANT ICI
#include <cstdlib>

CasePrison::CasePrison() : Case("Prison", -1), force_d1(0), force_d2(0), 
                           des_forces(false), utiliser_carte_reponse(false) {}

void CasePrison::emprisonner(Joueur* j) { if (j) liste_tour.emplace_back(j, 0); }

int& CasePrison::getTourRef(Joueur* j) {
    for (auto& p : liste_tour) if (p.first == j) return p.second;
    liste_tour.emplace_back(j, 0);
    return liste_tour.back().second;
}

int CasePrison::getTourPrison(Joueur* j) const {
    for (auto& p : liste_tour) if (p.first == j) return p.second;
    return 0;
}

void CasePrison::forcerResultatDes(int d1, int d2) { force_d1 = d1; force_d2 = d2; des_forces = true; }
void CasePrison::setUtiliserCarteReponse(bool rep) { utiliser_carte_reponse = rep; }

void CasePrison::action() {
    if (!joueur_actif) return;
    int& tours = getTourRef(joueur_actif);
    
    // CORRECTION ICI : Utilisation du vrai ID de la carte au lieu du chiffre '1'
    if (joueur_actif->possedeCarte(CarteSortiePrison::ID) && utiliser_carte_reponse) {
        joueur_actif->retirerCarte(CarteSortiePrison::ID); 
        tours = 0; 
        return;
    }
    
    int d1 = des_forces ? force_d1 : (std::rand() % 6 + 1);
    int d2 = des_forces ? force_d2 : (std::rand() % 6 + 1);
    if (d1 == d2) tours = 0;
    else { tours++; if (tours >= 3) tours = 0; }
}
