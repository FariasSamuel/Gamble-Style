/**
 * @file CarteDoubleGain.hpp
 * @brief Classe CarteDoubleGain.
 * @project GambleStyle — GM4 INSA Rouen Normandie
 */
#ifndef CARTEDOUBLEGAIN_HPP
#define CARTEDOUBLEGAIN_HPP

#include "Carte.hpp"

// ═══════════════════════════════════════════════════════════════
// CarteDoubleGain
// ═══════════════════════════════════════════════════════════════
/**
 * @class CarteDoubleGain
 * @brief Double les gains du joueur lors de sa prochaine victoire à un mini-jeu.
 */
class CarteDoubleGain : public Carte {
public:
    CarteDoubleGain(Joueur* joueur);
    void action() override;
private:
    Joueur* joueur;
};

#endif // CARTEDOUBLEGAIN_HPP
