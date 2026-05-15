// Carte.cpp
// Implémentation de la classe de base Carte.
// action() et setTitulaire() sont des no-ops ici ; toute carte concrète les surcharge.

#include "Carte.hpp"

Carte::Carte(const std::string& nom, const std::string& text)
    : nom_carte(nom), text(text)
{
}

Carte::~Carte() = default;

// No-op : les sous-classes définissent l'effet réel de la carte.
void Carte::action() {}

std::string Carte::get_nom_carte() const { return nom_carte; }
std::string Carte::get_nom_text()  const { return text; }
