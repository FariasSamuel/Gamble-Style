#ifndef JOUEUR_H
#define JOUEUR_H
#include <string>

class Case;        
class CasePropriete; 

class Joueur {
    //attribut
    private : const std::string nom;
              int capital;
              Case position;
              CasePropriete propriete[];
    
    //methodes

};


#endif