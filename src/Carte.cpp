/**
 * @file Carte.cpp
 * @brief Implémentation de la classe de base Carte.
 * @project GambleStyle — GM4 INSA Rouen Normandie
 */
#include "Carte.hpp"

Carte::Carte(const std::string& nom, const std::string& texte)
    : nom(nom), texte(texte) {}

const std::string& Carte::getNom()   const { return nom; }
const std::string& Carte::getTexte() const { return texte; }
