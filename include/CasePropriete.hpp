#ifndef CASEPROPRIETE_H
#define CASEPROPRIETE_H

#include "Case.hpp"
#include <string>

class Plateau;

class CasePropriete : public Case {
public:
    explicit CasePropriete(int num = 0);
    CasePropriete(const std::string& name, int prix);
    CasePropriete(int num, const std::string& name, int prix);
    ~CasePropriete() override;

    void action() override;

    int                getPrix()  const;
    const std::string& getName()  const;

    void    setProprietaire(Joueur* j);
    Joueur* getProprietaire() const;

    void setJoueurActif(Joueur* j);
    void setAchetable(bool val);

    // Câblage depuis buildGame()
    void setCommand(const std::string& cmd);
    void setPlateau(Plateau* p);

    // Flags interrogés par PlateauSFML après action()
    bool achatPropose()    const; // propriété non achetée : proposer l'achat
    bool gambleEnAttente() const; // propriétaire doit choisir sa mise via UI
    bool minijeuLance()    const; // le jeu a été lancé (après confirmerMise)

    // Appelé par PlateauSFML une fois la mise choisie par le propriétaire
    void confirmerMise(int mise); // lance le jeu, appelle repartition()

    // Accessible aux tests : répartition directe avec gagnant et mise connus
    // nullptr comme gagnant = égalité (aucun transfert)
    void repartition(Joueur* gagnant, int mise);

    // Gagnant du dernier jeu lancé (nullptr si égalité / pas encore joué)
    Joueur* getGagnantDernierJeu() const;

private:
    int         prix_achat;
    int         prix_hypotheque;
    bool        achetable;
    std::string name_;
    Joueur*     proprietaire;
    std::string command_;
    Plateau*    plateau_;

    bool    flag_achat_propose;
    bool    flag_gamble_attente;
    bool    flag_minijeu_lance;
    Joueur* dernierGagnant_{nullptr};
};

#endif // CASEPROPRIETE_H
