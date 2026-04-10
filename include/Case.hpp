#ifndef CASE_H
#define CASE_H
#include <string>

<<<<<<< HEAD
class Case {
    //attribut
    private: const int num_case;  // private ou protected ?? 
            
    //methodes
    public: Case(int num_case);
=======
class Joueur;

class Case {
    //attribut
    private: const int num_case;  // private ou protected ?? 
    protected: const Joueur *joueur[4]; // poiteur sur les joueurus pour y avoir acces
            
    //methodes
    public: Case(int num_case, Joueur *liste_joueurs[4]);
>>>>>>> b2acfafbc336c37558b23a8a560eaef08bd2cf57
            int get_num_case();
            virtual void action();
};


#endif