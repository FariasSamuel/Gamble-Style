#ifndef CARTEPOSITION_H
#define CARTEPOSITION_H

#include "Carte.hpp"
#include "Case.hpp"

class CartePosition : public Carte {
public:
    CartePosition(const std::string& nom = "", const std::string& text = "", Case* c = nullptr);
    ~CartePosition() override;

    void action() override; // deplace le joueur

private:
    Case* cible;
};

#endif // CARTEPOSITION_H