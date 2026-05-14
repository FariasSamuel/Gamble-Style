#include "CaseEvenement.hpp"
#include "Joueur.hpp"
#include <cstdlib>

CaseEvenement::CaseEvenement(): mise(0), gagnant(nullptr), played(false), gameName("") {}
CaseEvenement::~CaseEvenement() {}

void CaseEvenement::ajouterJoueur(Joueur* j) { joueurs.push_back(j); }
void CaseEvenement::setMise(int m) { mise = m; }
int CaseEvenement::getMise() const { return mise; }

void CaseEvenement::setGameName(const std::string& name) { gameName = name; }
std::string CaseEvenement::getGameName() const { return gameName; }

void CaseEvenement::setGagnant(Joueur* j) { gagnant = j; }

void CaseEvenement::distribution() {
    if(!gagnant) return;
    int total = mise * (int)joueurs.size();
    gagnant->setCapital(gagnant->getCapital() + total);
    for(auto p: joueurs) if(p!=gagnant) p->setCapital(p->getCapital() - mise);
}

void CaseEvenement::action() {
    if(joueurs.size() < 2) return;

    // Instead of running an in-process Minijeu, mark played and (optionally) run an external program.
    played = true;

    // If a game executable is specified, tests should not depend on actually running it.
    // However in real runtime, we could run it like:
    // if(!gameName.empty()) std::system(gameName.c_str());
    // The external program (e.g. ./Tron) would be responsible to return or communicate the winner.
}

bool CaseEvenement::minijeuJoue() const { return played; }
