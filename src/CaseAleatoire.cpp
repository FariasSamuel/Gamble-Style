#include "CaseAleatoire.hpp"
#include "Joueur.hpp"

CaseAleatoire::CaseAleatoire() {}
CaseAleatoire::~CaseAleatoire() {}

void CaseAleatoire::ajouterCarte(Carte* c) { file.push_back(c); }
Carte* CaseAleatoire::getPremiereCarteDeFile() { if(file.empty()) return nullptr; return file.front(); }
void CaseAleatoire::action() {
    if(file.empty()) return;
    Carte* c = file.front(); file.pop_front(); file.push_back(c);
}

void CaseAleatoire::setJoueurActif(Joueur* j) { setJoueurActif(j); }
