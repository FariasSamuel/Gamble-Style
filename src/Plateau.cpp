// Plateau.cpp
// Implémentation du moteur de jeu console (sans SFML).
// La logique d'affichage est dans PlateauSFML qui surcharge initialization()
// et gamelooping().

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
    // Les joueurs ne sont pas propriétaires ici ; seul PlateauSFML les détruit dans cleanupGame().
    joueurs.clear();
    // Les cases sont propriétées par le plateau.
    for (auto c : cases) delete c;
    cases.clear();
}

// No-op dans la classe de base ; PlateauSFML charge board.txt et construit les cases.
void Plateau::initialization() {}

// Retourne le joueur avec le score le plus élevé (capital + valeur propriétés).
// Utilisé pour désigner le vainqueur en fin de partie.
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

// Fait jouer un tour à chaque joueur encore solvable.
void Plateau::update()
{
    // Localise la case Prison pour la passer à tour()
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

// Boucle principale console : joue des tours tant qu'il reste plus d'un joueur solvable.
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

// Ajoute montant au capital de j (sans minimum).
void Plateau::donner_capital(Joueur* j, int montant)
{
    if (!j) return;
    j->setCapital(j->getCapital() + montant);
}

// Retire montant du capital de j. Si le capital passe en négatif, déclenche la banqueroute.
void Plateau::enlever_capital(Joueur* j, int montant)
{
    if (!j) return;
    j->setCapital(j->getCapital() - montant);
    if (j->getCapital() < 0) j->misebanqueroute();
}

// Transfère montant de src vers dst.
void Plateau::transfert_argent(Joueur* src, Joueur* dst, int montant)
{
    if (!src || !dst) return;
    enlever_capital(src, montant);
    donner_capital(dst, montant);
}

// Fait lancer les dés à chaque joueur, puis trie le vecteur par score décroissant.
// Le joueur avec le plus grand lancer joue en premier.
void Plateau::ordre()
{
    for (auto j : joueurs) if (j) j->lancerde();
    std::sort(joueurs.begin(), joueurs.end(), [](Joueur* a, Joueur* b) {
        return a->getDernierLancer() > b->getDernierLancer();
    });
}

void    Plateau::ajouterJoueur(Joueur* j)           { joueurs.push_back(j); }
Joueur* Plateau::getJoueur(size_t idx)        const  { if (idx < joueurs.size()) return joueurs[idx]; return nullptr; }
int     Plateau::nb_joueur()                  const  { return static_cast<int>(joueurs.size()); }
int     Plateau::get_temps_jeu()              const  { return compteur_tour; }
void    Plateau::setTempsJeu(int minutes)            { temps_jeu_ = minutes; }

void  Plateau::addCase(Case* c) { cases.push_back(c); }

// Cherche d'abord par num_case, puis par position dans le vecteur.
Case* Plateau::getCase(int idx) const
{
    for (auto c : cases) if (c && c->get_num_case() == idx) return c;
    if (idx >= 0 && idx < (int)cases.size()) return cases[idx];
    return nullptr;
}
