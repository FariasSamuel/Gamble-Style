#include "Plateau.hpp"
#include "Joueur.hpp"
#include "Case.hpp"

#include <algorithm>
#include <random>
#include <iostream>

Plateau::Plateau(float capital_initial_)
    : capital_initial(capital_initial_), compteur_tour(0)
{
}

Plateau::~Plateau()
{
    // cleanup
    joueurs.clear();
    cases.clear();
}

void Plateau::initialization()
{
#ifdef USE_SFML
    // SFML init (optional)
#endif
    // Fullscreen adaptatif
    VideoMode desktop = VideoMode::getDesktopMode();
    W = desktop.width;
    H = desktop.height;

    RenderWindow window(desktop, "Gamble style", Style::Fullscreen);
    window.setFramerateLimit(60);

    RenderTexture t;
    t.create(W, H);
    Sprite sprite(t.getTexture());
}

Joueur* Plateau::fin()
{
    // simple winner: highest capital + property value
    if (joueurs.empty()) return nullptr;
    Joueur* best = joueurs.front();
    int bestScore = best->getCapital() + best->getValPropriete();
    for (auto j : joueurs) {
        int score = j->getCapital() + j->getValPropriete();
        if (score > bestScore) { best = j; bestScore = score; }
    }
    return best;
}

void Plateau::gamelooping()
{
    for (Joueur* j : joueurs) {
        if (!j) continue;
        int de = j->lancerde();
        (void)de;
    }
    ++compteur_tour;
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
}

void Plateau::transfert_argent(Joueur* src, Joueur* dst, int montant)
{
    if (!src || !dst) return;
    enlever_capital(src, montant);
    donner_capital(dst, montant);
}

void Plateau::ordre()
{
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(joueurs.begin(), joueurs.end(), g);
}

void Plateau::ajouterJoueur(Joueur* j) { joueurs.push_back(j); }
Joueur* Plateau::getJoueur(size_t idx) const { if (idx < joueurs.size()) return joueurs[idx]; return nullptr; }
int Plateau::nb_joueur() const { return static_cast<int>(joueurs.size()); }
int Plateau::get_temps_jeu() const { return compteur_tour; }
