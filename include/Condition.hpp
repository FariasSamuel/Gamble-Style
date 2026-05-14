/**
 * @file Condition.hpp
 * @brief Énumération de l'état financier d'un Joueur.
 * @project GambleStyle — GM4 INSA Rouen Normandie
 */
#ifndef CONDITION_HPP
#define CONDITION_HPP

/**
 * @enum Condition
 * @brief État financier d'un joueur.
 */
enum class Condition {
    RICHE,       ///< capital >= 0
    BANQUEROUTE, ///< capital < 0, val_proprietes > 0
    FAILLITE     ///< capital de solvabilite <= 0, joueur éliminé
};

#endif // CONDITION_HPP
