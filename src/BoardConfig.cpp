// BoardConfig.cpp
// Lecture et parsing du fichier board.txt.
// Le format accepte des commentaires (#), des directives globales (grid_size,
// board_image, dark_coeff) et des lignes "case" décrivant chaque case.
//
// Ordre de lecture des cases : sens horaire depuis le coin bas-droit (case 0).

#include "BoardConfig.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

// Calcule le centre normalisé (x,y dans [0,1]) de la case idx sur une grille NxN.
// Les cases sont indexées en sens horaire depuis le coin bas-droit :
//   Rangée basse   (droite → gauche) : 0      ..  N-1
//   Colonne gauche (bas → haut)      : N      .. 2N-2
//   Rangée haute   (gauche → droite) : 2N-1   .. 3N-3
//   Colonne droite (haut → bas)      : 3N-2   .. 4N-5
std::pair<float, float> BoardConfig::caseNormPos(int idx) const
{
    int N = grid_size;
    int col, row;
    if (idx < N) {
        col = N - 1 - idx;          row = N - 1;
    } else if (idx < 2*N - 1) {
        col = 0;                     row = 2*N - 2 - idx;
    } else if (idx < 3*N - 2) {
        col = idx - (2*N - 2);      row = 0;
    } else {
        col = N - 1;                 row = idx - (3*N - 2) + 1;
    }
    return { (col + 0.5f) / N, (row + 0.5f) / N };
}

// Retourne l'index de la première case du type t, ou -1 si absente.
int BoardConfig::findCaseType(CaseType t) const
{
    for (int i = 0; i < (int)cases.size(); ++i)
        if (cases[i].type == t) return i;
    return -1;
}

// ── Helpers de parsing ────────────────────────────────────────────────────────

static CaseType parseCaseType(const std::string& s)
{
    if (s == "depart")      return CaseType::DEPART;
    if (s == "evenement")   return CaseType::EVENEMENT;
    if (s == "propriete")   return CaseType::PROPRIETE;
    if (s == "prison")      return CaseType::PRISON;
    if (s == "goto_prison") return CaseType::GOTO_PRISON;
    if (s == "aleatoire")   return CaseType::ALEATOIRE;
    return CaseType::EVENEMENT;  // type inconnu → événement par défaut
}

// Remplace les underscores par des espaces pour l'affichage dans l'UI.
static std::string deunderscore(const std::string& s)
{
    std::string r = s;
    std::replace(r.begin(), r.end(), '_', ' ');
    return r;
}

// ── Chargement du fichier ─────────────────────────────────────────────────────

// Lit board.txt ligne par ligne. Les lignes commençant par '#' sont ignorées.
// Retourne une configuration par défaut si le fichier est introuvable.
BoardConfig BoardConfig::loadFromFile(const std::string& path)
{
    BoardConfig cfg;
    std::ifstream f(path);
    if (!f.is_open()) {
        std::cerr << "[BoardConfig] Impossible d'ouvrir " << path
                  << " – config par défaut utilisée\n";
        return cfg;
    }

    std::string line;
    while (std::getline(f, line)) {
        // Supprime les commentaires en fin de ligne
        auto pos = line.find('#');
        if (pos != std::string::npos) line.erase(pos);

        std::istringstream ss(line);
        std::string key;
        if (!(ss >> key)) continue;

        if (key == "grid_size") {
            ss >> cfg.grid_size;
        } else if (key == "board_image") {
            ss >> cfg.board_image;
        } else if (key == "dark_coeff") {
            ss >> cfg.dark_coeff;
        } else if (key == "case") {
            CaseConfig cc;
            std::string typeStr, nameStr, texStr;
            ss >> typeStr >> nameStr >> texStr;
            cc.type    = parseCaseType(typeStr);
            cc.name    = deunderscore(nameStr);
            cc.texture = texStr;

            // 4e champ : entier optionnel (bonus, prix ou montant)
            // 5e champ : commande mini-jeu (PROPRIETE ou EVENEMENT uniquement)
            std::string extra;
            if (ss >> extra) {
                try { cc.param = std::stoi(extra); } catch (...) {}
                if (cc.type == CaseType::PROPRIETE || cc.type == CaseType::EVENEMENT) {
                    std::string cmd;
                    if (ss >> cmd) cc.command = cmd;
                }
            }
            if (cc.type == CaseType::EVENEMENT && cc.param <= 0)
                cc.param = 100;  // mise par défaut si non spécifiée

            cfg.cases.push_back(cc);
        }
    }

    // Vérification du nombre de cases attendu pour la grille déclarée
    int expected = cfg.nbCases();
    if ((int)cfg.cases.size() != expected)
        std::cerr << "[BoardConfig] " << expected << " cases attendues pour grid_size="
                  << cfg.grid_size << ", " << cfg.cases.size() << " lues\n";

    return cfg;
}
