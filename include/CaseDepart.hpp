#ifndef CASEDEPART_H
#define CASEDEPART_H
#include <string>
#include "Case.hpp"

class CaseDepart : public Case{
    //attribut
    private: int montant;
            
    //methodes
    public: CaseDepart(int get_num_case, int montant)
            void action();

};


#endif