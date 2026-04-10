#ifndef JOUEUR_H
#define JOUEUR_H
#include <string>

class Case;        
class CasePropriete; 
class Carte; 


enum class Condition {
    banqueroute,
    riche,
    faillite
};

class Joueur {
    //attribut
    private : const std::string nom;
              int capital;
              Case position;
              CasePropriete propriete[];
              Carte cartes[10];
              int conteur_double;
    
    //methodes

};


#endif