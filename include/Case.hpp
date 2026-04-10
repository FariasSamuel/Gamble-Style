#ifndef CASE_H
#define CASE_H
#include <string>


class Joueur;

class Case {
    //attribut
    protected : const int num_case;  //protected ?? 
            
    //methodes
    public : Case(int num_case);
            int get_num_case();
            virtual void action();
};


#endif