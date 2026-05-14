/**
 * @file CarteSortiePrison.hpp
 * @brief Classe CarteSortiePrison.
 * @project GambleStyle — GM4 INSA Rouen Normandie
 */
#ifndef CARTESORTIEPRISON_HPP
#define CARTESORTIEPRISON_HPP

#include "Carte.hpp"

// ═══════════════════════════════════════════════════════════════
// CarteSortiePrison
// ═══════════════════════════════════════════════════════════════
/**
 * @class CarteSortiePrison
 * @brief Permet au joueur de sortir de prison sans payer ni lancer les dés.
 */
class CarteSortiePrison : public Carte {
public:
    static const int ID = 101;
    CarteSortiePrison(Joueur* joueur);
    void action() override;
    int getId() const override { return ID; }
private:
    Joueur* joueur;
};

#endif // CARTESORTIEPRISON_HPP
