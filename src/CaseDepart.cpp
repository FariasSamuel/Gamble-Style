#include "CaseDepart.hpp"
#include "Joueur.hpp"

CaseDepart::CaseDepart(int montant_init, int num)
    : Case(num), montant(montant_init)
{
}

CaseDepart::~CaseDepart() = default;

void CaseDepart::action() {
    Joueur* j = getJoueurActif();
    if (!j) return;
    j->setCapital(j->getCapital() + montant);
}

int CaseDepart::getMontant() const { return montant; }

void CaseDepart::setJoueurActif(Joueur* j) { Case::setJoueurActif(j); }
