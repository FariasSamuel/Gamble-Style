// Joueur.cpp
// Implémentation de la classe Joueur : gestion du capital, des propriétés,
// des cartes, des déplacements et de l'état financier.

#include "Joueur.hpp"
#include "CasePropriete.hpp"
#include "Carte.hpp"
#include "CarteSortiePrison.hpp"
#include "CarteDoubleGain.hpp"
#include <cstdlib>
#include <algorithm>

Joueur::Joueur(const std::string& name, int capital_init)
    : nom(name), capital(capital_init), condition_(Condition::RICHE),
      position(nullptr), proprietes(), cartes(), compteur_double(0), lastLancer(0)
{
}

Joueur::~Joueur() = default;

const std::string& Joueur::getNom() const { return nom; }
int  Joueur::getCapital() const            { return capital; }
void Joueur::setCapital(int c)             { capital = c; }

// Lance un seul dé (1–6) et mémorise le résultat dans lastLancer.
int Joueur::lancerde()
{
    int v = (std::rand() % 6) + 1;
    lastLancer = v;
    return v;
}

int  Joueur::getDernierLancer() const { return lastLancer; }
int  Joueur::getCptPrison()     const { return cpt_prison_; }
void Joueur::setCptPrison(int v)      { cpt_prison_ = v; }

void  Joueur::bougerjoueur(Case* c)     { position = c; }
Case* Joueur::getCaseActuelle()   const { return position; }
void  Joueur::setCaseActuelle(Case* c)  { position = c; }

// Déduit le prix de la propriété du capital et l'ajoute à la liste.
void Joueur::acheter(CasePropriete* cp)
{
    if (!cp) return;
    capital -= cp->getPrix();
    proprietes.push_back(cp);
    cp->setProprietaire(this);
}

int Joueur::getNbProprietes() const { return static_cast<int>(proprietes.size()); }

// Retourne la somme des prix d'achat de toutes les propriétés détenues.
int Joueur::getValPropriete() const
{
    int sum = 0;
    for (auto p : proprietes) if (p) sum += p->getPrix();
    return sum;
}

// Perçoit la moitié du prix de cp, retire cp de la liste et la rend achetable.
void Joueur::hypotequer(CasePropriete* cp)
{
    if (!cp) return;
    capital += cp->getPrix() / 2;
    auto it = std::find(proprietes.begin(), proprietes.end(), cp);
    if (it != proprietes.end()) proprietes.erase(it);
    cp->setAchetable(true);
}

void Joueur::clearProprietes() { proprietes.clear(); }

void Joueur::donnerCarte(Carte* c)
{
    if (!c) return;
    cartes.push_back(c);
}

// Vérifie si le joueur possède une carte correspondant à l'identifiant id.
// Seul CarteSortiePrison::ID est géré ici ; étendre pour d'autres types si nécessaire.
bool Joueur::possedeCarte(int id) const
{
    for (auto c : cartes) {
        if (!c) continue;
        CarteSortiePrison* sp = dynamic_cast<CarteSortiePrison*>(c);
        if (sp && id == CarteSortiePrison::ID) return true;
    }
    return false;
}

// Retire c de l'inventaire sans le détruire (la case en est propriétaire).
void Joueur::retirerCarte(Carte* c)
{
    auto it = std::find(cartes.begin(), cartes.end(), c);
    if (it != cartes.end()) cartes.erase(it);
}

// Retourne la première CarteSortiePrison dans l'inventaire, ou nullptr.
CarteSortiePrison* Joueur::getSortiePrison() const
{
    for (auto c : cartes)
        if (auto* sp = dynamic_cast<CarteSortiePrison*>(c)) return sp;
    return nullptr;
}

// Retourne la première CarteDoubleGain dans l'inventaire, ou nullptr.
CarteDoubleGain* Joueur::getDoubleGain() const
{
    for (auto c : cartes)
        if (auto* dg = dynamic_cast<CarteDoubleGain*>(c)) return dg;
    return nullptr;
}

// Liquide toutes les propriétés à mi-prix et met à jour l'état financier.
// Si aucune propriété : FAILLITE immédiate. Sinon FAILLITE si capital reste ≤ 0.
void Joueur::misebanqueroute()
{
    if (proprietes.empty()) {
        condition_ = Condition::FAILLITE;
        capital = 0;
        return;
    }
    condition_ = Condition::BANQUEROUTE;
    for (auto p : proprietes) {
        if (p) {
            capital += p->getPrix() / 2;
            p->setAchetable(true);
        }
    }
    proprietes.clear();
    condition_ = (capital > 0) ? Condition::RICHE : Condition::FAILLITE;
}

// Retourne l'état financier :
//   - capital > 0  → RICHE
//   - condition_ marqué FAILLITE → FAILLITE
//   - sinon → BANQUEROUTE (capital ≤ 0, pas encore déclaré en faillite)
Condition Joueur::conditionfinanciere() const
{
    if (capital > 0) return Condition::RICHE;
    if (condition_ == Condition::FAILLITE) return Condition::FAILLITE;
    return Condition::BANQUEROUTE;
}

int Joueur::getCompteurDouble() const { return compteur_double; }

// Joue un tour complet en mode console : lance les dés, gère les doubles et
// la règle du triple double (→ prison), déplace le joueur et résout la case.
void Joueur::tour(const std::vector<Case*>& cases, int prisonIdx)
{
    if (cases.empty()) return;
    int nbCases = static_cast<int>(cases.size());

    compteur_double = 0;

    while (true) {
        int d1 = lancerde();
        int d2 = lancerde();
        bool estDouble = (d1 == d2);

        if (estDouble) ++compteur_double;

        // Triple double → envoi immédiat en prison, fin du tour
        if (compteur_double >= 3) {
            if (prisonIdx >= 0 && prisonIdx < nbCases) {
                position = cases[prisonIdx];
                cases[prisonIdx]->setJoueurActif(this);
                cases[prisonIdx]->action();
            }
            compteur_double = 0;
            return;
        }

        // Avancer du nombre de cases indiqué par les dés
        int posActuelle = 0;
        for (int i = 0; i < nbCases; ++i) {
            if (cases[i] == position) { posActuelle = i; break; }
        }
        int nouvellePos = (posActuelle + d1 + d2) % nbCases;
        position = cases[nouvellePos];
        cases[nouvellePos]->setJoueurActif(this);
        cases[nouvellePos]->action();

        if (!estDouble) {
            compteur_double = 0;
            return;
        }
        // Double → rejouer (boucle)
    }
}
