/**
 * @file CaseDepart.cpp
 * @brief Implémentation de CaseDepart.
 */
#include "CaseDepart.hpp"
#include "Joueur.hpp"

CaseDepart::CaseDepart(int montant)
    : Case("Depart", 0), montant(montant) {}

void CaseDepart::action() {
    if (joueur_actif) joueur_actif->donnerCapital(montant);
}

int CaseDepart::getMontant() const { return montant; }
