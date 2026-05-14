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

    void ajouterJoueur(Joueur* j);
    void setMise(int m);
    int getMise() const;

    // External game name to launch (e.g. "./Tron") instead of an in-process Minijeu
    void setGameName(const std::string& name);
    std::string getGameName() const;

    void setGagnant(Joueur* j);
    void distribution();
    void action() override;
    bool minijeuJoue() const;

private:
    std::vector<Joueur*> joueurs;
    int mise;
    Joueur* gagnant;
    bool played;
    std::string gameName; // name/path of external minigame executable
};

#endif // CASEEVENEMENT_H