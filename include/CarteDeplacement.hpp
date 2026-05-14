/**
 * @file CarteDeplacement.hpp
 * @brief Classe CarteDeplacement.
 * @project GambleStyle — GM4 INSA Rouen Normandie
 */
#ifndef CARTEDEPLACEMENT_HPP
#define CARTEDEPLACEMENT_HPP

#include "Carte.hpp"

// ═══════════════════════════════════════════════════════════════
// CarteDeplacement
// ═══════════════════════════════════════════════════════════════
/**
 * @class CarteDeplacement
 * @brief Déplace un joueur vers une case spécifique.
 */
class CarteDeplacement : public Carte {
public:
    CarteDeplacement(Joueur* joueur, Plateau* plateau, int caseIndex);
    void action() override;
private:
    Joueur* joueur;
    Plateau* plateau;
    int caseIndex;
};

#endif // CARTEDEPLACEMENT_HPP
