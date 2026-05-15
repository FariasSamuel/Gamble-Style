// Case.hpp
// Classe de base abstraite représentant une case du plateau.
// Toutes les cases spécialisées (CaseDepart, CasePropriete, etc.) en héritent.
// La relation de composition avec Joueur est non-propriétaire : la case ne
// détruit pas le joueur actif.

#ifndef CASE_H
#define CASE_H

#include <string>

class Joueur;

// Case — case générique identifiée par son index sur le plateau.
// action() est la méthode appelée lorsqu'un joueur atterrit sur cette case ;
// elle est virtuelle pure dans les sous-classes métier.
class Case {
public:
    explicit Case(int num = 0);
    virtual ~Case();

    // Retourne l'index de la case sur le plateau (0 = DÉPART par convention).
    int get_num_case() const;

    // Déclenche l'effet de la case sur le joueur actif. Implémentation vide ici.
    virtual void action();

    // Définit le joueur qui se trouve actuellement sur cette case.
    void setJoueurActif(Joueur* j);

    // Retourne le joueur actif (nullptr si aucun).
    Joueur* getJoueurActif() const;

    // Alias de get_num_case() pour la compatibilité avec les tests CppUnit.
    int getIndex() const;

private:
    int     num_case;      // index de la case dans le vecteur du plateau
    Joueur* joueur_actif;  // joueur dont c'est le tour sur cette case (non propriétaire)
};

#endif // CASE_H
