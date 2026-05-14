/**
 * @file CaseAleatoire.hpp
 * @brief Classe CaseAleatoire.
 * @project GambleStyle — GM4 INSA Rouen Normandie
 */
#ifndef CASEALEATOIRE_HPP
#define CASEALEATOIRE_HPP

#include "Case.hpp"
#include <vector>

class Carte;

// ═══════════════════════════════════════════════════════════════
// CaseAleatoire
// ═══════════════════════════════════════════════════════════════
/**
 * @class CaseAleatoire
 * @brief Tire la première carte d'une file circulaire et l'applique.
 */
class CaseAleatoire : public Case {
public:
    CaseAleatoire();
    virtual ~CaseAleatoire();
    void ajouterCarte(Carte* c);
    Carte* getPremiereCarteDeFile() const;
    void action() override;
private:
    std::vector<Carte*> liste_cartes;
};

#endif // CASEALEATOIRE_HPP
