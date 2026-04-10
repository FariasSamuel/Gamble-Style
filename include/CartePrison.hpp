#ifndef CARTEPRISON_H
#define CARTEPRISON_H
#include <string>
#include "Carte.hpp"


class CartePrison : public Carte{
    //attribut
            
    //methodes
    public: CartePrison(std::string nom_carte, std::string text); //
            void action(); //deplacer le joueur sur le CaseAleatoire sur la case prison



};


#endif