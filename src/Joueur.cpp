#include "Joueur.hpp"
#include "CasePropriete.hpp"
#include "Carte.hpp"
#include "CarteSortiePrison.hpp"
#include <cstdlib>

Joueur::Joueur(const std::string& name, int capital_init)
    : nom(name), capital(capital_init), position(nullptr), proprietes(), cartes(), compteur_double(0), lastLancer(0)
{
}

Joueur::~Joueur() = default;

const std::string& Joueur::getNom() const { return nom; }
int Joueur::getCapital() const { return capital; }
void Joueur::setCapital(int c) { capital = c; }

int Joueur::lancerde()
{
    int v = (std::rand() % 6) + 1;
    lastLancer = v;
    return v;
}

int Joueur::getDernierLancer() const { return lastLancer; }

void Joueur::bougerjoueur(Case* c) { position = c; }
Case* Joueur::getCaseActuelle() const { return position; }
void Joueur::setCaseActuelle(Case* c) { position = c; }

void Joueur::acheter(CasePropriete* cp)
{
    if (!cp) return;
    int prix = cp->getPrix();
    capital -= prix;
    proprietes.push_back(cp);
}

int Joueur::getNbProprietes() const { return static_cast<int>(proprietes.size()); }

int Joueur::getValPropriete() const
{
    int sum = 0;
    for (auto p : proprietes) if (p) sum += p->getPrix();
    return sum;
}

void Joueur::hypotequer(CasePropriete* cp)
{
    if (!cp) return;
    int remboursement = cp->getPrix() / 2;
    capital += remboursement;
}

void Joueur::clearProprietes() { proprietes.clear(); }

void Joueur::donnerCarte(Carte* c) {
    if (!c) return;
    cartes.push_back(c);
}

bool Joueur::possedeCarte(int id) const {
    for (auto c : cartes) {
        if (!c) continue;
        // check known ID
        CarteSortiePrison* sp = dynamic_cast<CarteSortiePrison*>(c);
        if (sp && id == CarteSortiePrison::ID) return true;
    }
    return false;
}

void Joueur::misebanqueroute()
{
    if (!proprietes.empty()) {
        for (auto p : proprietes) if (p) hypotequer(p);
        proprietes.clear();
        return;
    }

    // mark as bankrupt
    capital = -1000000;
}

Condition Joueur::conditionfinanciere() const
{
    if (capital > 0) return Condition::RICHE;
    if (capital < 0 && proprietes.empty()) return Condition::FAILLITE;
    return Condition::BANQUEROUTE;
}
