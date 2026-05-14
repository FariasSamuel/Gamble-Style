/**
 * @file Minijeu.hpp
 * @brief Classe Minijeu (implémentation par défaut aléatoire).
 * @project GambleStyle — GM4 INSA Rouen Normandie
 */
#ifndef MINIJEU_HPP
#define MINIJEU_HPP

class Joueur;

/**
 * @class Minijeu
 * @brief Mini-jeu jouable pour 2, 3 ou 4 joueurs.
 *
 * L'implémentation de base utilise un tirage aléatoire.
 * Les mini-jeux graphiques (Tron, Pong…) héritent de cette classe
 * et surchargent les méthodes jouer().
 */
class Minijeu {
public:
    Minijeu();
    virtual ~Minijeu() = default;

    virtual const char* getNom() const;

    /**
     * @brief Duel 2 joueurs → retourne le gagnant.
     * @post résultat != nullptr ; résultat == j1 || résultat == j2
     */
    virtual Joueur* jouer(Joueur* j1, Joueur* j2);

    /**
     * @brief 3 joueurs → tableau[3] trié du 1er au 3e, sans doublons.
     * @warning L'appelant libère le tableau avec delete[].
     */
    virtual Joueur** jouer(Joueur* j1, Joueur* j2, Joueur* j3);

    /**
     * @brief 4 joueurs → tableau[4] trié du 1er au 4e, sans doublons.
     * @warning L'appelant libère le tableau avec delete[].
     */
    virtual Joueur** jouer(Joueur* j1, Joueur* j2, Joueur* j3, Joueur* j4);
};

#endif // MINIJEU_HPP
