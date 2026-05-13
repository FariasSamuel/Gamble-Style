#include "Joueur.hpp"

Joueur::Joueur(const std::string& name, int capital_init)
    : nom(name), capital(capital_init), position(nullptr), proprietes(), cartes(), compteur_double(0)
{
}

Joueur::~Joueur() = default;

const std::string& Joueur::getNom() const { return nom; }

int Joueur::getCapital() const { return capital; }

void Joueur::setCapital(int c) { capital = c; }
