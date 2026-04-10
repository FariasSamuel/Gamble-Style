#ifndef CASEPROPRIETE_H
#define CASEPROPRIETE_H
#include <string>
#include "Case.hpp"

class CasePropriete : public Case{
    //attribut
    private: Minijeu jeu;
             int prix_achat;
             int prix_hypotheque;
             Joueur proprietaire;
             bool achetable;
            
    //methodes
    public: CasePropriete(int num_case, Minijeu jeu,int prix_achat, int prix_hypotheque); //initiallement achable à true, et pas de proprriétaire
            void gamble();
            void action();
            void repartition();

};


#endif