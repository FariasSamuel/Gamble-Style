#ifndef JOUEUR_H
#define JOUEUR_H
#include <string>
#include <vector>
#include "Case.hpp"

class Case;
class CasePropriete;
class Carte;
class CarteSortiePrison;
class CarteDoubleGain;

enum class Condition {
    RICHE,
    BANQUEROUTE,
    FAILLITE
};

class Joueur {
public:
    Joueur(const std::string& name, int capital_init = 1500);
    ~Joueur();

    const std::string& getNom() const;
    int getCapital() const;
    void setCapital(int c);

    int lancerde();
    int getDernierLancer() const;
    void bougerjoueur(Case* c);
    Case* getCaseActuelle() const;
    void setCaseActuelle(Case* c);

    void acheter(CasePropriete* cp);
    int getNbProprietes() const;
    int getValPropriete() const;
    void hypotequer(CasePropriete* cp);
    void clearProprietes();
    void misebanqueroute();
    Condition conditionfinanciere() const;

    void donnerCarte(Carte* c);
    bool possedeCarte(int id) const;
    void retirerCarte(Carte* c);
    CarteSortiePrison* getSortiePrison() const;
    CarteDoubleGain*   getDoubleGain()   const;

    void tour(const std::vector<Case*>& cases, int prisonIdx = -1);

    int getCompteurDouble() const;

    int  getCptPrison() const;
    void setCptPrison(int v);

private:
    const std::string nom;
    int capital;
    Condition condition_;
    Case* position;
    std::vector<CasePropriete*> proprietes;
    std::vector<Carte*> cartes;
    int compteur_double;
    int lastLancer{0};
    int cpt_prison_{0};
};

#endif
