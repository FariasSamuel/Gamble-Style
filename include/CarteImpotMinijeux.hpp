/**
 * @file CarteImpotMinijeux.hpp
 * @brief Classe CarteImpotMinijeux.
 * @project GambleStyle — GM4 INSA Rouen Normandie
 */
#ifndef CARTEIMPOTMINIJEUX_HPP
#define CARTEIMPOTMINIJEUX_HPP

#include "Carte.hpp"

// ═══════════════════════════════════════════════════════════════
// CarteImpotMinijeux
// ═══════════════════════════════════════════════════════════════
/**
 * @class CarteImpotMinijeux
 * @brief Prélève tauxParCase × nb_cases_possédées.
 */
class CarteImpotMinijeux : public Carte {
public:
    CarteImpotMinijeux(Joueur* joueur, Plateau* plateau, int tauxParCase = 25);
    void action() override;
private:
    Joueur* joueur;
    Plateau* plateau;
    int tauxParCase;
};

#endif // CARTEIMPOTMINIJEUX_HPP
