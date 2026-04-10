#ifndef JOUEUR_H
#define JOUEUR_H
#include <string>

class Case;        
class CasePropriete; 
<<<<<<< HEAD
class Carte; 


enum class Condition {
    banqueroute,
    riche,
    faillite
};
=======
>>>>>>> b2acfafbc336c37558b23a8a560eaef08bd2cf57

class Joueur {
    //attribut
    private : const std::string nom;
              int capital;
              Case position;
              CasePropriete propriete[];
<<<<<<< HEAD
              Carte cartes[10];
              int conteur_double;
=======
>>>>>>> b2acfafbc336c37558b23a8a560eaef08bd2cf57
    
    //methodes

};


#endif