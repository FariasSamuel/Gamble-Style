#include "BoardConfig.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

// ── Calcul de position ────────────────────────────────────────────────────────
// Cases indexées dans le sens horaire depuis le coin bas-droit.
// Grille NxN : 4*(N-1) cases sur le périmètre.
//
//   Rangée basse   (droite → gauche) : i = 0      ..  N-1
//   Colonne gauche (bas → haut)      : i = N      .. 2N-2
//   Rangée haute   (gauche → droite) : i = 2N-1   .. 3N-3
//   Colonne droite (haut → bas)      : i = 3N-2   .. 4N-5

std::pair<float, float> BoardConfig::caseNormPos(int idx) const
{
    int N = grid_size;
    int col, row;
    if (idx < N) {
        col = N - 1 - idx;  row = N - 1;
    } else if (idx < 2*N - 1) {
        col = 0;             row = 2*N - 2 - idx;
    } else if (idx < 3*N - 2) {
        col = idx - (2*N - 2); row = 0;
    } else {
        col = N - 1;         row = idx - (3*N - 2) + 1;
    }
    return { (col + 0.5f) / N, (row + 0.5f) / N };
}

int BoardConfig::findCaseType(CaseType t) const
{
    for (int i = 0; i < (int)cases.size(); ++i)
        if (cases[i].type == t) return i;
    return -1;
}

// ── Parser ────────────────────────────────────────────────────────────────────
static CaseType parseCaseType(const std::string& s)
{
    if (s == "depart")      return CaseType::DEPART;
    if (s == "evenement")   return CaseType::EVENEMENT;
    if (s == "propriete")   return CaseType::PROPRIETE;
    if (s == "prison")      return CaseType::PRISON;
    if (s == "goto_prison") return CaseType::GOTO_PRISON;
    if (s == "aleatoire")   return CaseType::ALEATOIRE;
    return CaseType::EVENEMENT;
}

// Underscores → espaces pour l'affichage en jeu.
static std::string deunderscore(const std::string& s)
{
    std::string r = s;
    std::replace(r.begin(), r.end(), '_', ' ');
    return r;
}

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
        // Supprimer les commentaires
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

            // 4e champ optionnel selon le type :
            //   EVENEMENT  → mise (int, défaut 100)
            //   PROPRIETE  → prix (int), puis 5e champ = commande du jeu
            //   DEPART     → bonus (int)
            std::string extra;
            if (ss >> extra) {
                try { cc.param = std::stoi(extra); } catch (...) {}
                if (cc.type == CaseType::PROPRIETE || cc.type == CaseType::EVENEMENT) {
                    std::string cmd;
                    if (ss >> cmd) cc.command = cmd;
                }
            }
            if (cc.type == CaseType::EVENEMENT && cc.param <= 0)
                cc.param = 100; // mise par défaut

            cfg.cases.push_back(cc);
        }
    }

    int expected = cfg.nbCases();
    if ((int)cfg.cases.size() != expected)
        std::cerr << "[BoardConfig] " << expected << " cases attendues pour grid_size="
                  << cfg.grid_size << ", " << cfg.cases.size() << " lues\n";

    return cfg;
}
