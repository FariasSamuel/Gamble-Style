#ifndef CARTEARGENT_H
#define CARTEARGENT_H
#include <string>
// #include "Joueur.hpp"
#include "Carte.hpp"


class CarteArgent : public Carte{
    //attribut
    private: const int montant;
             Joueur gagnants[4];
             Joueur perdants[4];

            
    //methodes
    public: CarteArgent(std::string nom_carte, std::string text, int montant); // declarer les liste de joueurs gagnats et perdant à null
            void ajouter_gagnant(Joueur j);
            void enlever_gagnant(Joueur j);
            void ajouter_perdant(Joueur j);
            void enlever_perdant(Joueur j);
            void action(); // faire les retraits et ajout d'argent



};


#endif