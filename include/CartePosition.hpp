#ifndef CARTEPOSITION_H
#define CARTEPOSITION_H
#include <string>
#include "Carte.hpp"


class CartePosition : public Carte{
    //attribut
    private: Case case;
            
    //methodes
    public: CartePosition(std::string nom_carte, std::string text, Case case); //
            void action(); //deplacer le joueur sur le CaseAleatoire sur la case prison



};


#endif