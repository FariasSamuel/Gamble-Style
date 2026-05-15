#ifndef CASEEVENEMENT_H
#define CASEEVENEMENT_H

#include <vector>
#include <string>
#include "Case.hpp"

class Joueur;

class CaseEvenement : public Case {
public:
    CaseEvenement();
    explicit CaseEvenement(int num);
    ~CaseEvenement() override;

    void    ajouterJoueur(Joueur* j);
    void    setMise(int m);
    int     getMise() const;
    void    setGagnant(Joueur* j);
    Joueur* getGagnant() const;
    void    setCommand(const std::string& cmd);

    // Chaque perdant transfère sa mise au gagnant
    void distribution();

    // Lance le programme externe (command_) et distribue les gains
    void action() override;

    bool minijeuJoue() const;

private:
    std::vector<Joueur*> joueurs;
    int         mise;
    Joueur*     gagnant;
    bool        played;
    std::string command_;
};

#endif // CASEEVENEMENT_H
