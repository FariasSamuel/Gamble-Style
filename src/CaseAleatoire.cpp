#include "CaseAleatoire.hpp"
#include "Carte.hpp"
#include "Joueur.hpp"

CaseAleatoire::CaseAleatoire() : Case(0) {}
CaseAleatoire::CaseAleatoire(int num) : Case(num) {}
CaseAleatoire::~CaseAleatoire() {}

void CaseAleatoire::ajouterCarte(Carte* c) { file.push_back(c); }
Carte* CaseAleatoire::getPremiereCarteDeFile() { if(file.empty()) return nullptr; return file.front(); }

void CaseAleatoire::action() {
    if(file.empty()) return;
    Carte* c = file.front(); file.pop_front(); file.push_back(c);
    c->action();
}

void CaseAleatoire::setJoueurActif(Joueur* j) { Case::setJoueurActif(j); }
