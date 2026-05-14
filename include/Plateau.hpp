#ifndef PLATEAU_H
#define PLATEAU_H

#include <vector>
#include <string>

class Joueur;
class Case;

class Plateau {
public:
    Plateau(float capital_initial = 1500.0f);
    ~Plateau();

    // initialisation: charge image et crée la fenêtre SFML (optionnel si SFML non disponible)
    void initialization();

    // nettoyage and compute winner
    Joueur* fin();

    // un tour de jeu: pour chaque joueur lancer et bouger
    void gamelooping();

    // gestion des capitaux
    void donner_capital(Joueur* j, int montant);
    void enlever_capital(Joueur* j, int montant);
    void transfert_argent(Joueur* src, Joueur* dst, int montant);

    // mélange l'ordre des joueurs
    void ordre();

    // add/get players for tests
    void ajouterJoueur(Joueur* j);
    Joueur* getJoueur(size_t idx) const;

    // legacy test-friendly names (without underscore)
    void donnercapital(Joueur* j, int montant) { donner_capital(j,montant); }
    void enlevercapital(Joueur* j, int montant) { enlever_capital(j,montant); }
    void transfertargent(Joueur* src, Joueur* dst, int montant) { transfert_argent(src,dst,montant); }

    // (no fin_retour; fin() returns the winner)

    // nombre de joueurs
    int nb_joueur() const;

    // accès au compteur de tours
    int get_temps_jeu() const;

private:
    std::vector<Joueur*> joueurs;
    std::vector<Case*> cases;
    float capital_initial;
    int compteur_tour;

#ifdef USE_SFML
    // forward-declare SFML types only if requested
    struct SFMLImpl;
    SFMLImpl* sfml;
#endif
};

#endif // PLATEAU_H
