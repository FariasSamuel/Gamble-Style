#ifndef CASEALEATOIRE_H
#define CASEALEATOIRE_H

#include "Case.hpp"

class CaseAleatoire : public Case {
public:
    explicit CaseAleatoire(int num = 0);
    ~CaseAleatoire() override;

    void action() override; // pioche une carte
};

#endif // CASEALEATOIRE_H
#ifndef CASEALEATOIRE_H
#define CASEALEATOIRE_H
#include <string>
#include "Case.hpp"

class CaseAleatoire : public Case{
    //attribut
<<<<<<< HEAD
    
    //methodes
    public: CaseAleatoire(int num_case);
            void action(); //fait tirer une carte, si l'instance de cette carte n'est poas Carte Prison ou CarteDoubleGain, faire carte.action(), sinon mettre la carte dans l'attribut 'carte[bcp de cartes]' du joueur
=======
            
    //methodes
>>>>>>> b2acfafbc336c37558b23a8a560eaef08bd2cf57

};


#endif