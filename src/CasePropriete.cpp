#include "CasePropriete.hpp"
#include "Joueur.hpp"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <vector>

CasePropriete::CasePropriete(int num)
    : Case(num), prix_achat(0), prix_hypotheque(0), achetable(true), name(""),
      proprietaire(nullptr), gamble_montant(0), gagnant(nullptr),
      flag_achat_propose(false), flag_minijeu_lance(false)
{
}

CasePropriete::CasePropriete(const std::string& name_, int prix)
    : Case(0), prix_achat(prix), prix_hypotheque(prix / 2), achetable(true), name(name_),
      proprietaire(nullptr), gamble_montant(0), gagnant(nullptr),
      flag_achat_propose(false), flag_minijeu_lance(false)
{
}

CasePropriete::CasePropriete(int num, const std::string& name_, int prix)
    : Case(num), prix_achat(prix), prix_hypotheque(prix / 2), achetable(true), name(name_),
      proprietaire(nullptr), gamble_montant(0), gagnant(nullptr),
      flag_achat_propose(false), flag_minijeu_lance(false)
{
}

CasePropriete::~CasePropriete() = default;

static Joueur* lancerTron(Joueur* j1, Joueur* j2)
{
    std::vector<Joueur*> participants = {j1, j2};
    FILE* pipe = popen("./Tron", "r");
    if (!pipe) return participants[std::rand() % 2];

    char buf[256] = {};
    if (fgets(buf, sizeof(buf), pipe)) {
        std::string nom(buf);
        nom.erase(std::remove(nom.begin(), nom.end(), '\n'), nom.end());
        nom.erase(std::remove(nom.begin(), nom.end(), '\r'), nom.end());
        pclose(pipe);
        for (auto j : participants) if (j && j->getNom() == nom) return j;
    } else {
        pclose(pipe);
    }
    return participants[std::rand() % 2];
}

void CasePropriete::action()
{
    Joueur* visiteur = getJoueurActif();

    if (!proprietaire) {
        flag_achat_propose = true;
        return;
    }

    // Visiteur = propriétaire → rien
    if (visiteur == proprietaire) return;

    // Duel : propriétaire fixe la mise, on lance Tron
    gamble();
    flag_minijeu_lance = true;

    Joueur* g = lancerTron(visiteur, proprietaire);
    setGagnant(g);
    repartition();
}

int CasePropriete::getPrix() const { return prix_achat; }

void CasePropriete::setProprietaire(Joueur* j) { proprietaire = j; }
Joueur* CasePropriete::getProprietaire() const { return proprietaire; }

bool CasePropriete::achatPropose() const { return flag_achat_propose; }
const std::string& CasePropriete::getName() const { return name; }

void CasePropriete::setJoueurActif(Joueur* j) { Case::setJoueurActif(j); }
void CasePropriete::setAchetable(bool val) { achetable = val; }

void CasePropriete::setGamble(int montant) { gamble_montant = montant; }
int CasePropriete::getGamble() const { return gamble_montant; }

void CasePropriete::setGagnant(Joueur* j) { gagnant = j; }
void CasePropriete::setGamblePredefini(int montant) { gamble_montant = montant; }
bool CasePropriete::minijeuLance() const { return flag_minijeu_lance; }

void CasePropriete::repartition()
{
    if (!gagnant) return;
    Joueur* visiteur = getJoueurActif();
    if (!visiteur) return;
    visiteur->setCapital(visiteur->getCapital() - gamble_montant);
    gagnant->setCapital(gagnant->getCapital() + gamble_montant);
}

void CasePropriete::gamble()
{
    if (gamble_montant <= 0) gamble_montant = 100;
}
