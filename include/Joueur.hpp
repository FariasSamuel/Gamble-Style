#ifndef JOUEUR_H
#define JOUEUR_H
#include <string>
#include <vector>

class Case;        
class CasePropriete; 
class Carte; 


enum class Condition {
    banqueroute,
    riche,
    faillite
};

class Joueur {
public:
    Joueur(const std::string& name, int capital_init = 1500);
    ~Joueur();

    const std::string& getNom() const;
    int getCapital() const;
    void setCapital(int c);

private:
    // attributs
    const std::string nom;
    int capital;
    Case* position;                // pointeur pour éviter type incomplet
    std::vector<CasePropriete*> proprietes;
    std::vector<Carte*> cartes;
    int compteur_double;
};


#endif