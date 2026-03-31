#ifndef CASE_H
#define CASE_H
#include <string>

class Joueur;

class Case {
    //attribut
    private: const int num_case;  // private ou protected ?? 
    protected: const Joueur *joueur[4]; // poiteur sur les joueurus pour y avoir acces
            
    //methodes
    public: Case(int num_case, Joueur *liste_joueurs[4]);
            int get_num_case();
            virtual void action();
};


#endif