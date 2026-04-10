#ifndef CARTEDOUBLEGAIN_H
#define CARTEDOUBLEGAIN_H
#include <string>
#include "Carte.hpp"


class CarteDoubleGain : public Carte{
    //attribut
            
    //methodes
    public: CarteDoubleGain(std::string nom_carte, std::string text); //
            void action(); //double la mise décidée du gamble

};


#endif