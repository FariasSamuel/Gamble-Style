#include "Carte.hpp"

Carte::Carte(const std::string& nom, const std::string& text)
    : nom_carte(nom), text(text)
{
}

Carte::~Carte() = default;

void Carte::action() { /* base no-op */ }

std::string Carte::get_nom_carte() const { return nom_carte; }

std::string Carte::get_nom_text() const { return text; }
