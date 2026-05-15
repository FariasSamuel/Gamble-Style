#pragma once
#include <string>
#include <vector>
#include <utility>

// Types de cases supportés dans board.txt.
enum class CaseType {
    DEPART,
    EVENEMENT,
    PROPRIETE,
    PRISON,
    GOTO_PRISON,
    ALEATOIRE
};

struct CaseConfig {
    CaseType    type;
    std::string name;      // underscores → espaces à l'affichage
    std::string texture;   // chemin relatif ou "none"
    std::string command;   // commande mini-jeu pour EVENEMENT (ex: "./Tron")
    int         param = 0; // bonus (DEPART) ou prix (PROPRIETE)
};

struct BoardConfig {
    int         grid_size   = 4;     // grille NxN carrée
    std::string board_image;         // image de fond ou "none"
    float       dark_coeff  = 0.40f; // assombrissement des cases (0=aucun, 1=noir)
    std::vector<CaseConfig> cases;

    // Nombre total de cases = 4*(grid_size-1)
    int nbCases() const { return 4 * (grid_size - 1); }

    // Centre normalisé (0..1) de la case i, sens horaire depuis coin bas-droit.
    std::pair<float, float> caseNormPos(int idx) const;

    // Premier index du type demandé, ou -1 si absent.
    int findCaseType(CaseType t) const;

    // Charge depuis un fichier texte ; retourne une config par défaut en cas d'erreur.
    static BoardConfig loadFromFile(const std::string& path);
};
