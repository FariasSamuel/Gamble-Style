#ifndef CASEEVENEMENT_H
#define CASEEVENEMENT_H
#include <string>
#include "Case.hpp"

class CaseEvenement : public Case{
    //attribut
    private: int montant;
             Minijeu minijeux[30];
            
    //methodes
    public: CaseEvenement(int num_case, int montant,Minijeu minijeux[30]);
            void modifier_montant(int nouv_montant); //necessaire ?
            void action();

};


#endif