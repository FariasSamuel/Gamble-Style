// CaseEvenement.hpp
// Case déclenchant un mini-jeu collectif (DiceBattle par défaut).
// Tous les joueurs inscrits participent ; le gagnant reçoit la mise de chacun
// des perdants via distribution().

#ifndef CASEEVENEMENT_H
#define CASEEVENEMENT_H

#include <vector>
#include <string>
#include "Case.hpp"

class Joueur;

// CaseEvenement — case "événement" lançant un mini-jeu entre tous les joueurs.
// PlateauSFML appelle ajouterJoueur() pour chaque participant puis action().
class CaseEvenement : public Case {
public:
    CaseEvenement();
    explicit CaseEvenement(int num);
    ~CaseEvenement() override;

    // Inscrit un joueur comme participant au mini-jeu.
    void ajouterJoueur(Joueur* j);

    // Fixe la mise engagée par chaque joueur (€).
    void setMise(int m);
    int  getMise() const;

    // Définit le gagnant manuellement (utilisé par les tests ou en cas de fallback).
    void    setGagnant(Joueur* j);
    Joueur* getGagnant() const;

    // Commande shell du mini-jeu (ex : "./bin/DiceBattle").
    void setCommand(const std::string& cmd);

    // Transfère la mise de chaque perdant vers le gagnant.
    void distribution();

    // Lance le programme externe désigné par command_, lit le nom du gagnant
    // sur stdout, puis appelle distribution(). Fallback aléatoire si introuvable.
    void action() override;

    // Retourne true si action() a déjà été appelé.
    bool minijeuJoue() const;

private:
    std::vector<Joueur*> joueurs;   // participants inscrits
    int         mise;               // montant engagé par participant (€)
    Joueur*     gagnant;            // vainqueur après résolution
    bool        played;             // verrou : empêche double exécution
    std::string command_;           // commande shell du mini-jeu
};

#endif // CASEEVENEMENT_H
