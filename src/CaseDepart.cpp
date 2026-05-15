// CaseDepart.cpp
// Implémentation de la case départ : verse un bonus au joueur qui l'atteint.
// Le bonus de passage (joueur qui survole la case) est géré côté PlateauSFML.

#include "CaseDepart.hpp"
#include "Joueur.hpp"

CaseDepart::CaseDepart(int montant_init, int num)
    : Case(num), montant(montant_init)
{
}

CaseDepart::~CaseDepart() = default;

// Verse montant € au joueur actif (atterrissage direct sur DÉPART).
void CaseDepart::action()
{
    Joueur* j = getJoueurActif();
    if (!j) return;
    j->setCapital(j->getCapital() + montant);
}

int  CaseDepart::getMontant()       const { return montant; }
void CaseDepart::setJoueurActif(Joueur* j) { Case::setJoueurActif(j); }
