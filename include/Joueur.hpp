#ifndef JOUEUR_H
#define JOUEUR_H
#include <string>

class Case;        
class CasePropriete; 
class Carte; 

enum class Condition {
    banqueroute,
    riche,
    faillite
};

class Joueur {
    //attribut
    private : const std::string nom;
              int capital;               // initialiser à 0
              Case position;             // initialiser vide
              CasePropriete propriete[]; // initialiser vide
              Carte cartes[10];          // initialiser vide
              int conteur_double;        // initialiser à 0
    
    //methodes
    public : Joueur(std::string nom);
             Condition condition_financiere();
             int val_propriete();
             void hypothequer(CasePropriete case);
             void acheter(CasePropriete case);
             void mise_banqueroute();
             int lancer_de();
             void tour();
             void bouger_joueur(Case case);

             void ajouter_capital(int montant);
             void enlever_capital(int montant);
             void ajouter_carte(Carte c);
             void enlever_carte(carte c);
             void incrementer_compteur(); //faire +1 dans le compteur lorsqu'on fait un double
             void compteur_nouveau(); //lorsqu'on veut remettre le compteur à zero
             void modifier_position(Case nouvelle_position);
             void ajouter_propriete(CasePropreiete case);
             void enlever_propriete(CasePropreiete case);

};


#endif