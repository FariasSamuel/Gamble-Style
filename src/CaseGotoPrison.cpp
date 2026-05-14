#include "CaseGotoPrison.hpp"
#include "CasePrison.hpp"
#include "Joueur.hpp"

CaseGotoPrison::CaseGotoPrison(int num, CasePrison* prison)
    : Case(num), prison_(prison) {}

CaseGotoPrison::~CaseGotoPrison() = default;

void CaseGotoPrison::setPrisonCase(CasePrison* p) { prison_ = p; }

void CaseGotoPrison::action()
{
    Joueur* j = getJoueurActif();
    if (!j || !prison_) return;
    // Move player to the prison tile; prison logic begins on next turn.
    j->bougerjoueur(prison_);
}
