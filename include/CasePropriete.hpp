#ifndef CASEPROPRIETE_H
#define CASEPROPRIETE_H

#include "Case.hpp"

class CasePropriete : public Case {
public:
    explicit CasePropriete(int num = 0);
    // alternate constructor used by tests (name, prix)
    CasePropriete(const std::string& name, int prix);
    ~CasePropriete() override;

    void action() override;

    int getPrix() const;

    void setProprietaire(class Joueur* j);
    Joueur* getProprietaire() const;

    bool achatPropose() const;

    void setJoueurActif(Joueur* j);

    void setGamble(int montant);
    int getGamble() const;
    void setGagnant(Joueur* j);
    void setGamblePredefini(int montant);
    bool minijeuLance() const;
    void repartition();
    void gamble();

private:
    int prix_achat;
    int prix_hypotheque;
    bool achetable;
    std::string name;
    Joueur* proprietaire;

    // gamble fields
    int gamble_montant;
    Joueur* gagnant;
    bool flag_achat_propose;
    bool flag_minijeu_lance;
};

#endif // CASEPROPRIETE_H