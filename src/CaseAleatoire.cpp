// CaseAleatoire.cpp
// Implémentation de la case pioche : rotation FIFO du deck de cartes.
// L'ordre est : setJoueurActif → getPremiereCarteDeFile (optionnel, pour l'UI) → action.

#include "CaseAleatoire.hpp"
#include "Carte.hpp"
#include "Joueur.hpp"

CaseAleatoire::CaseAleatoire()        : Case(0) {}
CaseAleatoire::CaseAleatoire(int num) : Case(num) {}

// Détruit toutes les cartes appartenant à cette case.
CaseAleatoire::~CaseAleatoire()
{
    for (auto c : file) delete c;
}

// Ajoute une carte à la fin du deck ; la case prend la propriété de l'objet.
void CaseAleatoire::ajouterCarte(Carte* c) { file.push_back(c); }

// Retourne un pointeur sur la prochaine carte sans la retirer du deck.
// PlateauSFML l'appelle avant action() pour préparer l'overlay d'affichage.
Carte* CaseAleatoire::getPremiereCarteDeFile()
{
    if (file.empty()) return nullptr;
    return file.front();
}

// Pioche la première carte, lui assigne le joueur actif, applique son effet,
// puis la remet en fin de file (rotation FIFO garantie).
void CaseAleatoire::action()
{
    if (file.empty()) return;
    Carte* c = file.front();
    file.pop_front();
    file.push_back(c);
    c->setTitulaire(getJoueurActif());
    c->action();
}

// Propage le joueur actif vers la classe de base avant d'appeler action().
void CaseAleatoire::setJoueurActif(Joueur* j) { Case::setJoueurActif(j); }
