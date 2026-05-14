#include "Joueur.hpp"
#include "CasePropriete.hpp"
#include "Carte.hpp"
#include "CarteSortiePrison.hpp"
#include <cstdlib>
#include <algorithm>

Joueur::Joueur(const std::string& name, int capital_init)
    : nom(name), capital(capital_init), condition_(Condition::RICHE),
      position(nullptr), proprietes(), cartes(), compteur_double(0), lastLancer(0)
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
    capital -= cp->getPrix();
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

bool Joueur::possedeCarte(int id) const
{
    for (auto c : cartes) {
        if (!c) continue;
        CarteSortiePrison* sp = dynamic_cast<CarteSortiePrison*>(c);
        if (sp && id == CarteSortiePrison::ID) return true;
    }
    return false;
}

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

Condition Joueur::conditionfinanciere() const
{
    if (capital > 0) return Condition::RICHE;
    if (condition_ == Condition::FAILLITE) return Condition::FAILLITE;
    return Condition::BANQUEROUTE;
}

int Joueur::getCompteurDouble() const { return compteur_double; }

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

        if (compteur_double >= 3) {
            // Triple double → prison
            if (prisonIdx >= 0 && prisonIdx < nbCases) {
                position = cases[prisonIdx];
                cases[prisonIdx]->setJoueurActif(this);
                cases[prisonIdx]->action();
            }
            compteur_double = 0;
            return;
        }

        // Avancer
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
        // Double → rejouer
    }
}
