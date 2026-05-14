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
    for (auto* j : joueurs) {
        delete j;
    }
    joueurs.clear();

    for (auto* c : cases) {
        delete c;
    }
    cases.clear();
}

void Plateau::initCases() {
    cases.push_back(new CaseDepart(200));
    for (int i = 1; i < 40; ++i)
        cases.push_back(new CasePropriete("Case " + std::to_string(i), 100 + i * 10));
}

void Plateau::ajouterJoueur(Joueur* j) { if (j) joueurs.push_back(j); }

void Plateau::retirerJoueursEnFaillite() {
    auto it = std::remove_if(joueurs.begin(), joueurs.end(), [](Joueur* j){
        if (j->conditionfinanciere() == Condition::FAILLITE) { delete j; return true; }
        return false;
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
    int actifs = 0;
    for (auto* j : joueurs) if (j->conditionfinanciere() != Condition::FAILLITE) ++actifs;
    return actifs <= 1;
}

Joueur* Plateau::fin() {
    if (joueurs.empty()) return nullptr;
    Joueur* g = joueurs[0];
    for (auto* j : joueurs) if (j->capitalSolvabilite() > g->capitalSolvabilite()) g = j;
    return g;
}

Joueur* Plateau::getJoueur(int idx) const { 
    return (idx >= 0 && idx < static_cast<int>(joueurs.size())) ? joueurs[idx] : nullptr; 
}
int Plateau::getNbJoueursActifs()   const { return joueurs.size(); }
Case* Plateau::getCase(int idx) const { 
    return (idx >= 0 && idx < static_cast<int>(cases.size())) ? cases[idx] : nullptr; 
}
int Plateau::getNbCases()           const { return cases.size(); }
