#ifndef CARTE_H
#define CARTE_H
#include <string>
// #include "Joueur.hpp"
#include "Case.hpp"


class Carte {
    //attribut
    protected: const std::string nom_carte;
             const std::string text;
            
    //methodes
    public: Carte(std::string nom_carte, std::string text);
            virtual void action();
};


#endif