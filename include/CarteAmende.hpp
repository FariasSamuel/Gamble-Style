/**
 * @file CarteAmende.hpp
 * @brief Classe CarteAmende.
 * @project GambleStyle — GM4 INSA Rouen Normandie
 */
#ifndef CARTEAMENDE_HPP
#define CARTEAMENDE_HPP

#include "Carte.hpp"

// ═══════════════════════════════════════════════════════════════
// CarteAmende
// ═══════════════════════════════════════════════════════════════
/**
 * @class CarteAmende
 * @brief Retire un montant fixe du capital du joueur.
 */
class CarteAmende : public Carte {
public:
    CarteAmende(Joueur* joueur, Plateau* plateau, int montant);
    void action() override;
private:
    Joueur* joueur;
    Plateau* plateau;
    int montant;
};

#endif // CARTEAMENDE_HPP
