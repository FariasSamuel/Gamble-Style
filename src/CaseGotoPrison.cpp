// CaseGotoPrison.cpp
// Case "Aller en Prison" : téléporte le joueur actif vers la case Prison
// et démarre son compteur d'emprisonnement.

#include "CaseGotoPrison.hpp"
#include "CasePrison.hpp"
#include "Joueur.hpp"

CaseGotoPrison::CaseGotoPrison(int num, CasePrison* prison)
    : Case(num), prison_(prison)
{}

CaseGotoPrison::~CaseGotoPrison() = default;

// Câble la case Prison après construction (appelé par buildGame()).
void CaseGotoPrison::setPrisonCase(CasePrison* p) { prison_ = p; }

// Téléporte le joueur actif sur la case prison_ sans passer par les cases intermédiaires.
// Le compteur de tours en prison est initialisé par PlateauSFML (setCptPrison(1)).
void CaseGotoPrison::action()
{
    Joueur* j = getJoueurActif();
    if (!j || !prison_) return;
    j->bougerjoueur(prison_);
}
