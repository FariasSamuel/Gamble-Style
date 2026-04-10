#ifndef CASEALEATOIRE_H
#define CASEALEATOIRE_H
#include <string>
#include "Case.hpp"

class CaseAleatoire : public Case{
    //attribut
    
    //methodes
    public: CaseAleatoire(int num_case);
            void action(); //fait tirer une carte, si l'instance de cette carte n'est poas Carte Prison ou CarteDoubleGain, faire carte.action(), sinon mettre la carte dans l'attribut 'carte[bcp de cartes]' du joueur

};


#endif