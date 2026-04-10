#ifndef PLATEAU_H
#define PLATEAU_H
#include <string>

class Case;        
class Joueur;


class Plateau {
    //attribut
    private : Joueur joueurs[4];
              Case cases[100];
              int capital_initial;
              int compteur_tours;
              int temps;
    
    //methodes
    public : Plateau();
             
             void ajouter_joueur(Joueur j);
             void ajouter_case(Case c);
             int get_capital_initial();
             int get_compteur_tour();
             int incrementer_compteur_tour(); //ajouter +1 au compteur_tours
             void set_temps(int tps);
             int get_temps();

             void initialisation();
             void ordre(Joueur joueur_ordre[4]);
             void nom_joueur(Joueur joueur_ordre[4]);
             int nb_joueur();
             int temps_jeu();
             void fin();
             void gamelooping();
             void update();
             void donner_capital(Joueur j, int montant);
             void enlever_capital(Joueur j, int montant);
             void transfert_argent(Joueur j_perd, Joueur j_recoit);

};


#endif