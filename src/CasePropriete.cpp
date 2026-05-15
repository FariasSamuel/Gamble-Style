/**
 * @file CasePropriete.cpp
 * @brief Implémentation de CasePropriete.
 */
#include "CasePropriete.hpp"
#include "Joueur.hpp"

CasePropriete::CasePropriete(const std::string& nom, int prix)
    : Case(nom, -1), prix_achat(prix), prix_hypotheque(prix / 2),
      proprietaire(nullptr), achetable(true), achat_propose(false),
      minijeu_lance(false), gamble_montant(0), gamble_predefini(0),
      mise_min(50), mise_max(200), gagnant(nullptr) {}

void CasePropriete::action() {
    achat_propose = false; minijeu_lance = false;
    if (!joueur_actif) return;
    if (!proprietaire) achat_propose = true;
    else if (proprietaire != joueur_actif) {
        minijeu_lance = true;
        gamble(joueur_actif);
    }
}

void CasePropriete::gamble() {
    int mise = gamble_predefini;
    if (mise < mise_min) mise = mise_min;
    if (mise > mise_max) mise = mise_max;
    gamble_montant = mise;
}

void CasePropriete::gamble(Joueur*) { gamble(); }

void CasePropriete::repartition() {
    if (!gagnant) return; 
    Joueur* perdant = (gagnant == proprietaire) ? joueur_actif : proprietaire;
    if (perdant) perdant->enleverCapital(gamble_montant);
    gagnant->donnerCapital(gamble_montant);
}
// Accesseurs et modificateurs (inchangés)
Joueur* CasePropriete::getProprietaire()   const { return proprietaire; }
int CasePropriete::getPrix()               const { return prix_achat; }
int CasePropriete::getPrixHypotheque()     const { return prix_hypotheque; }
bool CasePropriete::isAchetable()          const { return achetable; }
bool CasePropriete::achatPropose()         const { return achat_propose; }
bool CasePropriete::minijeuLance()         const { return minijeu_lance; }
int CasePropriete::getGamble()             const { return gamble_montant; }
void CasePropriete::setProprietaire(Joueur* j)   { proprietaire = j; }
void CasePropriete::setAchetable(bool v)         { achetable = v; }
void CasePropriete::setGamble(int m)             { gamble_montant = m; }
void CasePropriete::setGagnant(Joueur* j)        { gagnant = j; }
void CasePropriete::setGamblePredefini(int v)    { gamble_predefini = v; }
void CasePropriete::setMiseLimites(int mn, int mx){ mise_min = mn; mise_max = mx; }
