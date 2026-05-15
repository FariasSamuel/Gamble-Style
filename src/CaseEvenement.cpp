// CaseEvenement.cpp
// Implémentation de la case événement : lance DiceBattle (ou tout autre programme)
// entre tous les joueurs actifs et redistribue la mise au gagnant.

#include "CaseEvenement.hpp"
#include "Joueur.hpp"
#include <cstdio>
#include <cstdlib>
#include <algorithm>

CaseEvenement::CaseEvenement()
    : Case(0), mise(100), gagnant(nullptr), played(false), command_("") {}

CaseEvenement::CaseEvenement(int num)
    : Case(num), mise(100), gagnant(nullptr), played(false), command_("") {}

CaseEvenement::~CaseEvenement() {}

void    CaseEvenement::ajouterJoueur(Joueur* j)           { joueurs.push_back(j); }
void    CaseEvenement::setMise(int m)                     { mise = m; }
int     CaseEvenement::getMise()                    const { return mise; }
void    CaseEvenement::setGagnant(Joueur* j)              { gagnant = j; }
Joueur* CaseEvenement::getGagnant()                 const { return gagnant; }
void    CaseEvenement::setCommand(const std::string& cmd) { command_ = cmd; }

// Transfère mise € de chaque perdant vers le gagnant.
void CaseEvenement::distribution()
{
    if (!gagnant) return;
    for (auto p : joueurs) {
        if (p && p != gagnant) {
            p->setCapital(p->getCapital() - mise);
            gagnant->setCapital(gagnant->getCapital() + mise);
        }
    }
}

// Lance command_ avec les noms des joueurs actifs en argv.
// Lit le nom du gagnant sur stdout, puis distribue les gains.
// Fallback aléatoire si popen() échoue ou si command_ est vide.
void CaseEvenement::action()
{
    played  = true;
    gagnant = nullptr;

    // Filtre les joueurs encore solvables
    std::vector<Joueur*> actifs;
    for (auto j : joueurs)
        if (j && j->conditionfinanciere() != Condition::FAILLITE)
            actifs.push_back(j);

    if (actifs.size() < 2) {
        if (!actifs.empty()) gagnant = actifs[0];
        return;
    }

    if (command_.empty()) {
        // Mode sans mini-jeu : gagnant aléatoire
        gagnant = actifs[std::rand() % actifs.size()];
    } else {
        // Construit la commande : cmd "Joueur1" "Joueur2" ...
        std::string full = command_;
        for (auto j : actifs)
            full += " \"" + j->getNom() + "\"";

        FILE* pipe = popen(full.c_str(), "r");
        if (!pipe) {
            gagnant = actifs[std::rand() % actifs.size()];
        } else {
            char buf[256] = {};
            if (fgets(buf, sizeof(buf), pipe)) {
                std::string nom(buf);
                nom.erase(std::remove(nom.begin(), nom.end(), '\n'), nom.end());
                nom.erase(std::remove(nom.begin(), nom.end(), '\r'), nom.end());
                for (auto j : actifs) {
                    if (j->getNom() == nom) { gagnant = j; break; }
                }
            }
            pclose(pipe);
        }
    }

    distribution();
}

bool CaseEvenement::minijeuJoue() const { return played; }
