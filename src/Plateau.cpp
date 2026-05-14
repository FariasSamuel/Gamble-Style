#include "Plateau.hpp"
#include "Joueur.hpp"
#include "Case.hpp"
#include "CasePrison.hpp"

#include <algorithm>
#include <iostream>

Plateau::Plateau(float capital_initial_)
    : capital_initial(capital_initial_), compteur_tour(0), temps_jeu_(0)
{
}

Plateau::~Plateau()
{
    joueurs.clear();
    cases.clear();
}

void Plateau::initialization()
{
    // Initialisation terminale : saisie nb joueurs, noms, ordre, temps
    // (l'initialisation graphique SFML est gérée dans main.cpp)
}

Joueur* Plateau::fin()
{
    if (joueurs.empty()) return nullptr;
    Joueur* best = joueurs.front();
    int bestScore = best->getCapital() + best->getValPropriete();
    for (auto j : joueurs) {
        int score = j->getCapital() + j->getValPropriete();
        if (score > bestScore) { best = j; bestScore = score; }
    }
    return best;
}

void Plateau::update()
{
    int prisonIdx = -1;
    for (int i = 0; i < (int)cases.size(); ++i) {
        if (dynamic_cast<CasePrison*>(cases[i])) { prisonIdx = i; break; }
    }

    std::vector<Joueur*> actifs;
    for (auto j : joueurs)
        if (j && j->conditionfinanciere() != Condition::FAILLITE) actifs.push_back(j);

    for (auto j : actifs)
        j->tour(cases, prisonIdx);

    ++compteur_tour;
}

void Plateau::gamelooping()
{
    auto nbActifs = [&]() {
        int n = 0;
        for (auto j : joueurs)
            if (j && j->conditionfinanciere() != Condition::FAILLITE) ++n;
        return n;
    };

    while (nbActifs() > 1)
        update();
}

void Plateau::donner_capital(Joueur* j, int montant)
{
    if (!j) return;
    j->setCapital(j->getCapital() + montant);
}

void Plateau::enlever_capital(Joueur* j, int montant)
{
    if (!j) return;
    j->setCapital(j->getCapital() - montant);
    if (j->getCapital() < 0) j->misebanqueroute();
}

void Plateau::transfert_argent(Joueur* src, Joueur* dst, int montant)
{
    if (!src || !dst) return;
    enlever_capital(src, montant);
    donner_capital(dst, montant);
}

void Plateau::ordre()
{
    for (auto j : joueurs) if (j) j->lancerde();
    std::sort(joueurs.begin(), joueurs.end(), [](Joueur* a, Joueur* b) {
        return a->getDernierLancer() > b->getDernierLancer();
    });
}

void Plateau::ajouterJoueur(Joueur* j) { joueurs.push_back(j); }
Joueur* Plateau::getJoueur(size_t idx) const { if (idx < joueurs.size()) return joueurs[idx]; return nullptr; }
int Plateau::nb_joueur() const { return static_cast<int>(joueurs.size()); }
int Plateau::get_temps_jeu() const { return compteur_tour; }
void Plateau::setTempsJeu(int minutes) { temps_jeu_ = minutes; }

void Plateau::addCase(Case* c) { cases.push_back(c); }
Case* Plateau::getCase(int idx) const
{
    for (auto c : cases) if (c && c->get_num_case() == idx) return c;
    if (idx >= 0 && idx < (int)cases.size()) return cases[idx];
    return nullptr;
}
