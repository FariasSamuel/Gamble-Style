#include "CartePosition.hpp"

CartePosition::CartePosition(const std::string& nom, const std::string& text, Case* c)
    : Carte(nom, text), cible(c)
{
}

CartePosition::~CartePosition() = default;

void CartePosition::action() {
    // to implement
}
