#include "CaseEvenement.hpp"
#include "Joueur.hpp"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <algorithm>

CaseEvenement::CaseEvenement(): Case(0), mise(0), gagnant(nullptr), played(false), gameName("./Tron") {}
CaseEvenement::CaseEvenement(int num): Case(num), mise(0), gagnant(nullptr), played(false), gameName("./Tron") {}
CaseEvenement::~CaseEvenement() {}

void CaseEvenement::ajouterJoueur(Joueur* j) { joueurs.push_back(j); }
void CaseEvenement::setMise(int m) { mise = m; }
int CaseEvenement::getMise() const { return mise; }

void CaseEvenement::setGameName(const std::string& name) { gameName = name; }
std::string CaseEvenement::getGameName() const { return gameName; }

void CaseEvenement::setGagnant(Joueur* j) { gagnant = j; }

void CaseEvenement::distribution()
{
    if (!gagnant) return;
    int total = mise * static_cast<int>(joueurs.size());
    // 1er reçoit 3/4, 2e reçoit 1/4 (tous les joueurs contribuent la mise)
    for (auto p : joueurs) if (p != gagnant) p->setCapital(p->getCapital() - mise);
    gagnant->setCapital(gagnant->getCapital() + total * 3 / 4);
}

static Joueur* lancerMinijeu(const std::string& gameName, const std::vector<Joueur*>& joueurs)
{
    if (joueurs.empty()) return nullptr;

    FILE* pipe = popen(gameName.c_str(), "r");
    if (!pipe) {
        // fallback aléatoire si ./Tron introuvable
        return joueurs[std::rand() % joueurs.size()];
    }

    char buf[256] = {};
    if (fgets(buf, sizeof(buf), pipe)) {
        std::string nomGagnant(buf);
        // Supprimer le saut de ligne final
        nomGagnant.erase(std::remove(nomGagnant.begin(), nomGagnant.end(), '\n'), nomGagnant.end());
        nomGagnant.erase(std::remove(nomGagnant.begin(), nomGagnant.end(), '\r'), nomGagnant.end());
        pclose(pipe);
        for (auto j : joueurs) {
            if (j && j->getNom() == nomGagnant) return j;
        }
    } else {
        pclose(pipe);
    }

    // Nom non reconnu → fallback aléatoire
    return joueurs[std::rand() % joueurs.size()];
}

void CaseEvenement::action()
{
    if (joueurs.size() < 2) return;
    played = true;
    Joueur* g = lancerMinijeu(gameName, joueurs);
    setGagnant(g);
    distribution();
}

bool CaseEvenement::minijeuJoue() const { return played; }
