#ifndef CASE_H
#define CASE_H

#include <string>

class Joueur;

class Case {
public:
    explicit Case(int num = 0);
    virtual ~Case();

    int get_num_case() const;
    virtual void action();
    
    void setJoueurActif(Joueur* j);
    Joueur* getJoueurActif() const;
    int getIndex() const; // alias to get_num_case for tests

private:
    int num_case;
    Joueur* joueur_actif;
};

#endif // CASE_H