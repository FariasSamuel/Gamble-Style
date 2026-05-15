// BoardConfig.hpp
// Lecture et représentation de la configuration du plateau depuis board.txt.
// BoardConfig est une structure de données pure (pas de logique de jeu).
// PlateauSFML l'utilise dans buildGame() pour instancier les cases.
//
// Format board.txt :
//   grid_size N          → plateau NxN, soit 4*(N-1) cases
//   board_image <path>   → image de fond
//   dark_coeff X         → assombrissement des cases (0.0–1.0)
//   case <type> <nom> <texture> [param] [command]

#pragma once

#include <string>
#include <vector>
#include <utility>

// Types de cases reconnus dans board.txt.
enum class CaseType {
    DEPART,       // case de départ, avec bonus de passage
    EVENEMENT,    // mini-jeu collectif (DiceBattle)
    PROPRIETE,    // case achetable avec mini-jeu 1v1 (Tron)
    PRISON,       // station prison (visite ou emprisonnement)
    GOTO_PRISON,  // case "Aller en Prison" (téléporte vers PRISON)
    ALEATOIRE     // pioche de carte aléatoire
};

// Configuration d'une case individuelle telle que lue dans board.txt.
struct CaseConfig {
    CaseType    type;       // type de la case
    std::string name;       // nom affiché (underscores → espaces)
    std::string texture;    // chemin de l'image ou "none"
    std::string command;    // commande mini-jeu (PROPRIETE / EVENEMENT)
    int         param = 0;  // bonus (DEPART, €) ou prix d'achat (PROPRIETE, €)
                            // ou montant des cartes (ALEATOIRE, €)
};

// Configuration globale du plateau chargée depuis un fichier texte.
struct BoardConfig {
    int         grid_size   = 4;      // taille de la grille NxN
    std::string board_image;          // chemin de l'image de fond (ou "none")
    float       dark_coeff  = 0.40f;  // coefficient d'assombrissement des cases

    std::vector<CaseConfig> cases;    // liste ordonnée des cases (sens horaire)

    // Retourne le nombre total de cases (4 * (grid_size - 1)).
    int nbCases() const { return 4 * (grid_size - 1); }

    // Retourne le centre normalisé (x,y dans [0,1]) de la case i, sens horaire depuis coin bas-droit.
    std::pair<float, float> caseNormPos(int idx) const;

    // Retourne le premier index d'une case du type t, ou -1 si absent.
    int findCaseType(CaseType t) const;

    // Charge la configuration depuis path ; retourne une config par défaut si erreur.
    static BoardConfig loadFromFile(const std::string& path);
};
