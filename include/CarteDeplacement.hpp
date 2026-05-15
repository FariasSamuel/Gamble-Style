// CarteDeplacement.hpp
// Carte "Déplacement" : avance le titulaire sur le plateau.
// Deux modes selon le constructeur utilisé :
//   - Legacy (tests) : téléportation vers un index absolu fixe.
//   - Gameplay       : avance d'un nombre de cases aléatoire dans [minSteps, maxSteps].

#ifndef CARTEDEPLACEMENT_H
#define CARTEDEPLACEMENT_H

#include "Carte.hpp"

class Plateau;
class Joueur;
class Case;

// CarteDeplacement — déplacement du titulaire sur le plateau.
// Si le joueur repasse par la case 0 (DÉPART), le bonus de passage est géré
// par PlateauSFML après le retour d'action().
class CarteDeplacement : public Carte {
public:
    // Constructeur legacy (tests) : téléporte vers la case d'index indexCase.
    CarteDeplacement(Joueur* titulaire, Plateau* plateau, int indexCase);

    // Constructeur gameplay : avance de minSteps à maxSteps cases au hasard.
    CarteDeplacement(Plateau* plateau, int minSteps, int maxSteps);

    ~CarteDeplacement() override;

    // Déplace le titulaire : vers caseTarget_ (legacy) ou d'un nombre aléatoire de cases.
    void action() override;

    // Mis à jour par CaseAleatoire::action() avant chaque tirage.
    void setTitulaire(Joueur* j) override { titulaire_ = j; }

private:
    Joueur*  titulaire_;        // joueur à déplacer (non propriétaire)
    Plateau* plateau_;          // plateau source des cases (non propriétaire)
    Case*    caseTarget_;       // case cible fixe (non nullptr en mode legacy, nullptr en gameplay)
    int      minSteps_{0};      // avance minimale en mode gameplay
    int      maxSteps_{0};      // avance maximale en mode gameplay
};

#endif // CARTEDEPLACEMENT_H
