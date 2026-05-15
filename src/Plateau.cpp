/**
 * @file Plateau.cpp
 * @brief Implémentation de la classe Plateau.
 */
#include "Plateau.hpp"
#include "Joueur.hpp"
#include "CaseDepart.hpp"
#include "CasePropriete.hpp"
#include <algorithm>

Plateau::Plateau() { initCases(); }

Plateau::~Plateau() {
    for (Case* c : cases) delete c;
    joueurs.clear(); // On vide la liste, mais on ne fait pas "delete" sur les joueurs
}

void Plateau::initCases() {
    // 1. On crée les cases
    cases.push_back(new CaseDepart(200));
    for (int i = 1; i < 40; ++i) {
        cases.push_back(new CasePropriete("Case " + std::to_string(i), 100 + i * 10));
    }
    
    // 2. On assigne officiellement son numéro d'index à chaque case
    for (size_t i = 0; i < cases.size(); ++i) {
        cases[i]->setIndex(i);
    }
}

void Plateau::ajouterJoueur(Joueur* j) { 
    if (!j) return;
    // Sécurité : on n'ajoute le joueur que s'il n'est pas déjà présent
    for (auto* existing : joueurs) {
        if (existing == j) return; 
    }
    joueurs.push_back(j); 
}

void Plateau::retirerJoueursEnFaillite() {
    auto it = std::remove_if(joueurs.begin(), joueurs.end(), [](Joueur* j){
        // On ne retire QUE ceux qui sont en FAILLITE (plus d'argent ET plus de biens)
        return (j->conditionfinanciere() == Condition::FAILLITE);
    });
    joueurs.erase(it, joueurs.end());
}

void Plateau::donnercapital(Joueur* j, int m)    { if(j) j->donnerCapital(m); }
void Plateau::enlevercapital(Joueur* j, int m)   { if(j) j->enleverCapital(m); }
void Plateau::transfertargent(Joueur* j1, Joueur* j2, int m) {
    if(j1 && j2) { j1->enleverCapital(m); j2->donnerCapital(m); }
}

void Plateau::ordre() {
    for (auto* j : joueurs) j->setDernierLancer(j->lancerde() + j->lancerde());
    trierParDernierLancer();
}

void Plateau::trierParDernierLancer() {
    std::sort(joueurs.begin(), joueurs.end(), [](Joueur* a, Joueur* b){
        return a->getDernierLancer() > b->getDernierLancer();
    });
}

void Plateau::update() {
    for (auto* j : joueurs) {
        if (j->conditionfinanciere() != Condition::FAILLITE) {
            j->tour(); // Fait jouer le tour au joueur et incrémente son compteur
        }
    }
}

void Plateau::gamelooping() { while (!finDePartie()) update(); }

bool Plateau::finDePartie() const {
    // La partie finit s'il reste 1 seul joueur (ou 0 par erreur)
    return (joueurs.size() <= 1);
}

Joueur* Plateau::fin() const {
    if (joueurs.empty()) return nullptr;
    
    // Le gagnant est celui qui a la plus grande solvabilité (cash + valeur propriétés)
    Joueur* gagnant = joueurs[0];
    for (Joueur* j : joueurs) {
        if (j->capitalSolvabilite() > gagnant->capitalSolvabilite()) {
            gagnant = j;
        }
    }
    return gagnant;
}

Joueur* Plateau::getJoueur(int idx) const { 
    return (idx >= 0 && idx < static_cast<int>(joueurs.size())) ? joueurs[idx] : nullptr; 
}
int Plateau::getNbJoueursActifs()   const { return joueurs.size(); }
Case* Plateau::getCase(int idx) const { 
    return (idx >= 0 && idx < static_cast<int>(cases.size())) ? cases[idx] : nullptr; 
}
int Plateau::getNbCases()           const { return cases.size(); }
