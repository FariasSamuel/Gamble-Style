/**
 * @file CarteGain.hpp
 * @brief Classe CarteGain.
 * @project GambleStyle — GM4 INSA Rouen Normandie
 */
#ifndef CARTEGAIN_HPP
#define CARTEGAIN_HPP

#include "Carte.hpp"

// ═══════════════════════════════════════════════════════════════
// CarteGain
// ═══════════════════════════════════════════════════════════════
/**
 * @class CarteGain
 * @brief Crédite un montant fixe au capital du joueur.
 */
class CarteGain : public Carte {
public:
    CarteGain(Joueur* joueur, Plateau* plateau, int montant);
    void action() override;
private:
    Joueur* joueur;
    Plateau* plateau;
    int montant;
};

#endif // CARTEGAIN_HPP
