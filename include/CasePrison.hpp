/**
 * @file CasePrison.hpp
 * @brief Classe CasePrison.
 * @project GambleStyle — GM4 INSA Rouen Normandie
 */
#ifndef CASEPRISON_HPP
#define CASEPRISON_HPP

#include "Case.hpp"
#include <vector>
#include <utility>

// ═══════════════════════════════════════════════════════════════
// CasePrison
// ═══════════════════════════════════════════════════════════════
/**
 * @class CasePrison
 * @brief Bloque le joueur jusqu'à double, carte, ou 3 tours.
 */
class CasePrison : public Case {
public:
    CasePrison();
    void action() override;

    void emprisonner(Joueur* j);
    int  getTourPrison(Joueur* j) const;

    /** @brief Pour les tests : force le résultat des dés. */
    void forcerResultatDes(int d1, int d2);
    /** @brief Pour les tests : simule la réponse du joueur pour la carte. */
    void setUtiliserCarteReponse(bool rep);

private:
    std::vector<std::pair<Joueur*, int>> liste_tour;
    int force_d1, force_d2;
    bool des_forces;
    bool utiliser_carte_reponse;

    int& getTourRef(Joueur* j);
};

#endif // CASEPRISON_HPP
