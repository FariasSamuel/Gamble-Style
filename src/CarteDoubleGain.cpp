// CarteDoubleGain.cpp
// Carte conservée en main. action() la place dans l'inventaire du titulaire.
// consommer() la désactive après usage (la case en reste propriétaire).

#include "CarteDoubleGain.hpp"
#include "Joueur.hpp"

CarteDoubleGain::CarteDoubleGain(const std::string& nom, const std::string& text)
    : Carte(nom, text)
{
}

CarteDoubleGain::~CarteDoubleGain() = default;

// Remet la carte dans l'inventaire du titulaire et l'active.
void CarteDoubleGain::action()
{
    if (titulaire_) titulaire_->donnerCarte(this);
    active_ = true;
}

Joueur* CarteDoubleGain::getTitulaire() const { return titulaire_; }
bool    CarteDoubleGain::estActive()    const { return active_; }

// Désactive la carte après utilisation ; elle reste en mémoire (propriété de la case).
void CarteDoubleGain::consommer() { active_ = false; }
