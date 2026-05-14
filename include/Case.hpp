/**
 * @file Case.hpp
 * @brief Classe abstraite Case.
 * @project GambleStyle — GM4 INSA Rouen Normandie
 */
#ifndef CASE_HPP
#define CASE_HPP

#include <string>

class Joueur;

// ═══════════════════════════════════════════════════════════════
// Classe abstraite Case
// ═══════════════════════════════════════════════════════════════
/**
 * @class Case
 * @brief Case abstraite du plateau. Chaque sous-classe implémente action().
 */
class Case {
public:
    Case(const std::string& nom, int index);
    explicit Case(int index);          ///< Constructeur minimal (nom vide)
    virtual ~Case() = default;

    const std::string& getNom()  const;
    int getIndex()               const;

    void  setJoueurActif(Joueur* j);
    Joueur* getJoueurActif()     const;

    /** @brief Déclenche l'effet de la case sur le joueur actif. */
    virtual void action() = 0;

protected:
    std::string nom;
    int index;
    Joueur* joueur_actif;
};

#endif // CASE_HPP
