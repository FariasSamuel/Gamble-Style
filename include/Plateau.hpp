#ifndef PLATEAU_H
#define PLATEAU_H

#include <vector>
#include <string>

class Joueur;
class Case;

class Plateau {
public:
    Plateau(float capital_initial = 1500.0f);
    virtual ~Plateau();

    void initialization();

    Joueur* fin();

    void update();
    void gamelooping();

    void donner_capital(Joueur* j, int montant);
    void enlever_capital(Joueur* j, int montant);
    void transfert_argent(Joueur* src, Joueur* dst, int montant);

    void ordre();

    void ajouterJoueur(Joueur* j);
    Joueur* getJoueur(size_t idx) const;

    void addCase(Case* c);
    Case* getCase(int idx) const;

    // legacy aliases
    void donnercapital(Joueur* j, int montant) { donner_capital(j, montant); }
    void enlevercapital(Joueur* j, int montant) { enlever_capital(j, montant); }
    void transfertargent(Joueur* src, Joueur* dst, int montant) { transfert_argent(src, dst, montant); }

    int nb_joueur() const;
    int nbCases()   const { return static_cast<int>(cases.size()); }
    int get_temps_jeu() const;
    void setTempsJeu(int minutes);

protected:
    std::vector<Joueur*> joueurs;
    std::vector<Case*>   cases;

private:
    float capital_initial;
    int   compteur_tour;
    int   temps_jeu_;
};

#endif // PLATEAU_H
