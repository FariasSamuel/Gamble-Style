#ifndef JOUEUR_H
#define JOUEUR_H
#include <string>
#include <vector>
#include "Case.hpp"

class Case;
class CasePropriete;
class Carte;


// Condition values expected by tests (scoped enum)
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

    // interactions required by Plateau
    int lancerde();
    int getDernierLancer() const;
    void bougerjoueur(Case* c);
    Case* getCaseActuelle() const;
    void setCaseActuelle(Case* c);

    // business methods used by tests
    void acheter(class CasePropriete* cp);
    int getNbProprietes() const;
    int getValPropriete() const;
    void hypotequer(class CasePropriete* cp);
    void clearProprietes();
    void misebanqueroute();
    Condition conditionfinanciere() const;
    // card management used by tests
    void donnerCarte(Carte* c);
    bool possedeCarte(int id) const;

private:
    // attributs
    const std::string nom;
    int capital;
    Case* position;                // pointeur pour éviter type incomplet
    std::vector<CasePropriete*> proprietes;
    std::vector<Carte*> cartes;
    int compteur_double;
    int lastLancer{0};
};


#endif