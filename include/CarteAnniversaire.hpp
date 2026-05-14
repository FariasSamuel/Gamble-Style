/**
 * @file CarteAnniversaire.hpp
 * @brief Classe CarteAnniversaire.
 * @project GambleStyle — GM4 INSA Rouen Normandie
 */
#ifndef CARTEANNIVERSAIRE_HPP
#define CARTEANNIVERSAIRE_HPP

#include "Carte.hpp"

// ═══════════════════════════════════════════════════════════════
// CarteAnniversaire
// ═══════════════════════════════════════════════════════════════
/**
 * @class CarteAnniversaire
 * @brief Chaque autre joueur verse un montant fixe au titulaire.
 */
class CarteAnniversaire : public Carte {
public:
    CarteAnniversaire(Joueur* titulaire, Plateau* plateau, int montant);
    void action() override;
private:
    Joueur* titulaire;
    Plateau* plateau;
    int montant;
};

#endif // CARTEANNIVERSAIRE_HPP
