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
    public: Carte(string nom_carte, string text);
            std::string get_nom_carte();
            std::string get_nom_text();
            virtual void action();
};


#endif