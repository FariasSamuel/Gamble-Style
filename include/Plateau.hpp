// Plateau.hpp
// Classe de base du plateau de jeu : contient la liste des joueurs, la liste
// des cases, et toute la logique métier indépendante de l'affichage.
//
// PlateauSFML hérite de Plateau et ajoute la fenêtre SFML et la boucle de rendu.
// Plateau lui-même est 100 % testable sans dépendance à SFML.
//
// Cycle d'utilisation minimal (console) :
//   Plateau p;
//   p.ajouterJoueur(new Joueur("Alice"));
//   p.initialization();   // lit board.txt (no-op dans la base)
//   p.gamelooping();      // boucle jusqu'à fin de partie

#ifndef PLATEAU_H
#define PLATEAU_H

#include <vector>
#include <string>

class Joueur;
class Case;

// Plateau — moteur de jeu sans interface graphique.
// Les méthodes initialization() et gamelooping() sont vides ici ;
// PlateauSFML les surcharge pour piloter la fenêtre SFML.
class Plateau {
public:
    explicit Plateau(float capital_initial = 1500.0f);
    virtual ~Plateau();

    // ── Initialisation ──────────────────────────────────────────────────────

    // Lit board.txt et construit les cases. Surchargé dans PlateauSFML.
    void initialization();

    // ── Fin de partie ───────────────────────────────────────────────────────

    // Retourne le seul joueur encore solvable (RICHE), ou nullptr si partie non terminée.
    Joueur* fin();

    // ── Boucle de jeu ───────────────────────────────────────────────────────

    // Joue un tour pour chaque joueur actif. Utilisé en mode console.
    void update();

    // Boucle principale : appelle update() jusqu'à fin de partie. Surchargé dans PlateauSFML.
    void gamelooping();

    // ── Gestion du capital ──────────────────────────────────────────────────

    // Ajoute montant € au capital de j (jamais négatif ici).
    void donner_capital(Joueur* j, int montant);

    // Retire montant € du capital de j.
    void enlever_capital(Joueur* j, int montant);

    // Transfère montant € de src vers dst.
    void transfert_argent(Joueur* src, Joueur* dst, int montant);

    // Alias snake_case pour la compatibilité avec les tests CppUnit.
    void donnercapital(Joueur* j, int montant)                   { donner_capital(j, montant); }
    void enlevercapital(Joueur* j, int montant)                  { enlever_capital(j, montant); }
    void transfertargent(Joueur* src, Joueur* dst, int montant)  { transfert_argent(src, dst, montant); }

    // ── Joueurs ─────────────────────────────────────────────────────────────

    // Détermine l'ordre de jeu initial en faisant lancer les dés à chaque joueur,
    // puis trie le vecteur par score décroissant.
    void ordre();

    // Ajoute un joueur au plateau (le plateau ne prend pas la propriété).
    void ajouterJoueur(Joueur* j);

    // Retourne le joueur à l'index idx, ou nullptr si hors bornes.
    Joueur* getJoueur(size_t idx) const;

    // Nombre de joueurs enregistrés.
    int nb_joueur() const;

    // ── Cases ───────────────────────────────────────────────────────────────

    // Ajoute une case au plateau (le plateau prend la propriété de l'objet).
    void addCase(Case* c);

    // Retourne la case à l'index idx, ou nullptr si hors bornes.
    Case* getCase(int idx) const;

    // Nombre de cases sur le plateau.
    int nbCases() const { return static_cast<int>(cases.size()); }

    // ── Durée de partie ─────────────────────────────────────────────────────

    int  get_temps_jeu() const;       // durée maximale en minutes (0 = illimitée)
    void setTempsJeu(int minutes);

protected:
    std::vector<Joueur*> joueurs;  // liste des joueurs (non propriétaires)
    std::vector<Case*>   cases;    // liste des cases   (propriétaires)

private:
    float capital_initial;  // capital de départ distribué à chaque joueur
    int   compteur_tour;    // nombre de tours joués depuis le début
    int   temps_jeu_;       // durée max en minutes (0 = sans limite)
};

#endif // PLATEAU_H
