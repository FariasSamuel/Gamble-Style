/**
 * @file Carte.hpp
 * @brief Classe abstraite Carte.
 * @project GambleStyle — GM4 INSA Rouen Normandie
 */
#ifndef CARTE_HPP
#define CARTE_HPP

#include <string>

class Joueur;
class Plateau;
class Case;

// ═══════════════════════════════════════════════════════════════
// Classe abstraite Carte
// ═══════════════════════════════════════════════════════════════
/**
 * @class Carte
 * @brief Carte tirée sur une CaseAleatoire. Chaque sous-classe implémente action().
 */
class Carte {
public:
    Carte(const std::string& nom, const std::string& texte);
    virtual ~Carte() = default;

    const std::string& getNom()   const;
    const std::string& getTexte() const;

    virtual void action() = 0;
    virtual int getId() const { return 0; }

protected:
    std::string nom;
    std::string texte;
};

#endif // CARTE_HPP
