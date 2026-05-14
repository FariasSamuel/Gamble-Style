/**
 * @file Plateau.hpp
 * @brief Déclaration de la classe Plateau.
 * @project GambleStyle — GM4 INSA Rouen Normandie
 */
#ifndef PLATEAU_HPP
#define PLATEAU_HPP

#include <vector>

class Joueur;
class Case;

/**
 * @class Plateau
 * @brief Chef d'orchestre de la partie GambleStyle.
 */
class Plateau {
public:
    Plateau();
    ~Plateau();

    // ── Initialisation ────────────────────────────────────────
    void ajouterJoueur(Joueur* j);
    void retirerJoueursEnFaillite();

    // ── Gestion des capitaux ──────────────────────────────────
    void donnercapital(Joueur* j, int montant);
    void enlevercapital(Joueur* j, int montant);
    void transfertargent(Joueur* j1, Joueur* j2, int montant);

    // ── Ordre et tours ────────────────────────────────────────
    void ordre();
    void trierParDernierLancer();
    void update();
    void gamelooping();
    bool finDePartie() const;

    // ── Fin de partie ─────────────────────────────────────────
    Joueur* fin();

    // ── Accesseurs ────────────────────────────────────────────
    Joueur* getJoueur(int index) const;
    int getNbJoueursActifs()     const;
    Case* getCase(int index)   const;
    int getNbCases()             const;

private:
    std::vector<Joueur*> joueurs;
    std::vector<Case*>   cases;
    void initCases();
};

#endif // PLATEAU_HPP
