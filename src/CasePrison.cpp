#include "CasePrison.hpp"
#include "Joueur.hpp"
#include "CarteSortiePrison.hpp"

CasePrison::CasePrison(int num)
    : Case(num), utiliserCarteReponse_(false), forced_d1_(0), forced_d2_(0), has_forced_(false)
{
}

CasePrison::~CasePrison() = default;

void CasePrison::setUtiliserCarteReponse(bool val)
{
    utiliserCarteReponse_ = val;
}

void CasePrison::forcerResultatDes(int d1, int d2)
{
    forced_d1_ = d1;
    forced_d2_ = d2;
    has_forced_ = true;
}

int CasePrison::getTourPrison(Joueur* j)
{
    auto it = tours_.find(j);
    if (it == tours_.end()) return 0;
    return it->second;
}

void CasePrison::action()
{
    Joueur* j = getJoueurActif();
    if (!j) return;

    // if player has a get-out-of-jail card and chooses to use it
    if (j->possedeCarte(CarteSortiePrison::ID) && utiliserCarteReponse_) {
        // consume card - naive: remove one such card
        // Joueur::misebanqueroute or card removal not implemented fully; assume possession resets
        // reset prison turn
        tours_[j] = 0;
        return;
    }

    int d1, d2;
    if (has_forced_) {
        d1 = forced_d1_;
        d2 = forced_d2_;
        // résultat persistant : on ne consomme pas has_forced_
    } else {
        d1 = j->lancerde();
        d2 = j->lancerde();
    }

    if (d1 == d2) {
        tours_[j] = 0;
        return;
    }

    tours_[j] = getTourPrison(j) + 1;
    if (tours_[j] >= 3) {
        tours_[j] = 0;
    }
}

