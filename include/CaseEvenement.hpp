/**
 * @file CaseEvenement.hpp
 * @brief Classe CaseEvenement.
 * @project GambleStyle — GM4 INSA Rouen Normandie
 */
#ifndef CASEEVENEMENT_HPP
#define CASEEVENEMENT_HPP

#include "Case.hpp"
#include <vector>

class Minijeu;

// ═══════════════════════════════════════════════════════════════
// CaseEvenement
// ═══════════════════════════════════════════════════════════════
/**
 * @class CaseEvenement
 * @brief Déclenche un mini-jeu pour tous les joueurs. Distribution 3/4 – 1/4.
 */
class CaseEvenement : public Case {
public:
    CaseEvenement();
    void action() override;

    void ajouterJoueur(Joueur* j);
    void setMise(int m);
    void setGagnant(Joueur* j);
    void setClassement(const std::vector<Joueur*>& cl);
    void clearMinijeux();

    Minijeu* choixminijeu();
    void distribution();
    bool minijeuJoue() const;

private:
    std::vector<Joueur*> joueurs;
    std::vector<Minijeu*> minijeux;
    int mise;
    Joueur* gagnant;
    std::vector<Joueur*> classement;
    bool minijeu_joue;
};

#endif // CASEEVENEMENT_HPP
