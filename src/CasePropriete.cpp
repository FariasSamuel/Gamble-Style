#include "CasePropriete.hpp"
#include "Joueur.hpp"

CasePropriete::CasePropriete(int num)
    : Case(num), prix_achat(0), prix_hypotheque(0), achetable(true), name(""), proprietaire(nullptr), gamble_montant(0), gagnant(nullptr), flag_achat_propose(false), flag_minijeu_lance(false)
{
}

CasePropriete::CasePropriete(const std::string& name_, int prix)
    : Case(0), prix_achat(prix), prix_hypotheque(0), achetable(true), name(name_), proprietaire(nullptr), gamble_montant(0), gagnant(nullptr), flag_achat_propose(false), flag_minijeu_lance(false)
{
}

CasePropriete::~CasePropriete() = default;

void CasePropriete::action() {
    // If there's no owner, propose purchase
    if (!proprietaire) {
        flag_achat_propose = true;
        return;
    }

    // If owner exists and gamble is set, mark minijeu launched
    if (gamble_montant > 0) {
        flag_minijeu_lance = true;
    }
}

int CasePropriete::getPrix() const { return prix_achat; }

void CasePropriete::setProprietaire(Joueur* j) { proprietaire = j; }

Joueur* CasePropriete::getProprietaire() const { return proprietaire; }

bool CasePropriete::achatPropose() const { return flag_achat_propose; }

void CasePropriete::setJoueurActif(Joueur* j) { Case::setJoueurActif(j); }

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
    // If a predefined amount exists, use it; otherwise pick a default positive amount
    if (gamble_montant <= 0) gamble_montant = 100; // default
}
