#ifndef CARTEDOUBLEGAIN_H
#define CARTEDOUBLEGAIN_H

#include "Carte.hpp"

class Joueur;

class CarteDoubleGain : public Carte {
public:
    CarteDoubleGain(const std::string& nom = "", const std::string& text = "");
    ~CarteDoubleGain() override;

    void action() override;

    void setTitulaire(Joueur* j);
    Joueur* getTitulaire() const;
    bool estActive() const;
    void consommer();

private:
    Joueur* titulaire_{nullptr};
    bool active_{false};
};

#endif // CARTEDOUBLEGAIN_H