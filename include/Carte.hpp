// Carte.hpp
// Classe de base pour toutes les cartes pouvant être tirées ou conservées
// par un joueur. Le polymorphisme repose sur action() et setTitulaire().
//
// Hiérarchie :
//   Carte
//     ├── CarteAnniversaire  — chaque adversaire verse un montant au titulaire
//     ├── CarteArgent        — gain ou perte aléatoire d'un montant fixe
//     ├── CarteDoubleGain    — carte conservée ; double la mise si elle est utilisée avant un jeu
//     ├── CarteDeplacement   — avance le joueur de N cases au hasard (ou vers un index fixe)
//     └── CarteSortiePrison  — carte conservée ; libère le joueur sans lancer les dés

#ifndef CARTE_H
#define CARTE_H

#include <string>

class Joueur;

// Carte — base commune à toutes les cartes du jeu.
// setTitulaire() est appelé par CaseAleatoire::action() juste avant chaque tirage
// afin que la carte connaisse le joueur actif, même si elle a été créée avec titulaire=nullptr.
class Carte {
public:
    Carte(const std::string& nom = "", const std::string& text = "");
    virtual ~Carte();

    // Applique l'effet de la carte sur son titulaire. À surcharger.
    virtual void action();

    // Assigne le joueur bénéficiaire. Implémentation vide ici ; les sous-classes stockent le pointeur.
    virtual void setTitulaire(Joueur*) {}

    // Retourne le nom court de la carte (affiché dans le log et l'UI).
    std::string get_nom_carte() const;

    // Retourne le texte descriptif de la carte.
    std::string get_nom_text() const;

private:
    std::string nom_carte;  // identifiant court (ex : "Anniversaire")
    std::string text;       // description affichée au joueur
};

#endif // CARTE_H
