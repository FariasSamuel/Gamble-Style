#ifndef CASEALEATOIRE_H
#define CASEALEATOIRE_H

#include "Case.hpp"
#include <deque>

class Carte;
class Joueur;

class CaseAleatoire : public Case {
public:
    CaseAleatoire();
    explicit CaseAleatoire(int num);
    ~CaseAleatoire();
    void ajouterCarte(Carte* c);
    Carte* getPremiereCarteDeFile();
    void action();
    void setJoueurActif(Joueur* j);
private:
    std::deque<Carte*> file;
};
#endif