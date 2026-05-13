#ifndef CARTEPRISON_H
#define CARTEPRISON_H

#include "Carte.hpp"

class CartePrison : public Carte {
public:
    CartePrison(const std::string& nom = "", const std::string& text = "");
    ~CartePrison() override;

    void action() override; // donne une carte sortie prison ou similar
};

#endif // CARTEPRISON_H