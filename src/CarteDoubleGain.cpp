#include "CarteDoubleGain.hpp"
#include "Joueur.hpp"

CarteDoubleGain::CarteDoubleGain(const std::string& nom, const std::string& text)
    : Carte(nom, text)
{
}

CarteDoubleGain::~CarteDoubleGain() = default;

void CarteDoubleGain::action() {
    if (titulaire_) titulaire_->donnerCarte(this);
    active_ = true;
}

void CarteDoubleGain::setTitulaire(Joueur* j) { titulaire_ = j; }
Joueur* CarteDoubleGain::getTitulaire() const { return titulaire_; }
bool CarteDoubleGain::estActive() const { return active_; }
void CarteDoubleGain::consommer() { active_ = false; }
