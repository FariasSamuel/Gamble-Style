#pragma once
#include "Plateau.hpp"
#include "BoardConfig.hpp"
#include "Joueur.hpp"
#include <SFML/Graphics.hpp>
#include <deque>
#include <string>
#include <vector>

class CasePropriete;
class CaseGotoPrison;

// ─────────────────────────────────────────────────────────────────────────────
//  PlateauSFML
//
//  Hérite de Plateau (logique métier, 100 % testable sans SFML).
//  Ajoute la fenêtre SFML, les textures, et la boucle de jeu visuelle.
//
//  Cycle d'utilisation :
//      PlateauSFML p;
//      p.initialization();   // lit board.txt, affiche l'écran de config
//      p.gamelooping();      // boucle SFML jusqu'à fermeture de la fenêtre
// ─────────────────────────────────────────────────────────────────────────────
class PlateauSFML : public Plateau
{
public:
    PlateauSFML();
    ~PlateauSFML() override;

    // Crée la fenêtre, charge les assets, affiche l'écran de configuration,
    // puis construit les objets de jeu (cases, joueurs).
    void initialization();

    // Boucle principale SFML (événements + rendu) jusqu'à fermeture.
    void gamelooping();

private:
    // ── Fenêtre & rendu ───────────────────────────────────────────────────────
    sf::RenderWindow window_;
    sf::Font         font_;
    bool             hasFont_ = false;

    // ── Configuration & textures ──────────────────────────────────────────────
    BoardConfig cfg_;

    sf::Texture              texBoard_;
    bool                     hasBoardTex_ = false;
    sf::Sprite               spriteBoard_;
    std::vector<sf::Texture> cellTex_;
    std::vector<bool>        hasCellTex_;
    sf::Texture              texJoueur_[4];
    sf::Texture              texDe_[6];

    // ── Mise en page (adaptative au grid_size) ────────────────────────────────
    static constexpr unsigned BOARD_SIZE = 670u;
    static constexpr unsigned PANEL_W    = 420u;
    static constexpr unsigned LOG_H      = 130u;
    static constexpr unsigned WIN_W      = BOARD_SIZE + PANEL_W;  // 1090
    static constexpr unsigned WIN_H      = BOARD_SIZE + LOG_H;    // 800
    float cellPx_ = 0.f;

    // ── Écran de configuration (données saisies) ──────────────────────────────
    int         nbJoueurs_   = 2;
    std::string noms_[4]     = {"Joueur 1", "Joueur 2", "Joueur 3", "Joueur 4"};
    std::string dureeStr_    = "30";
    int         activeField_ = -1; // -1=aucun, 0=durée, 1-4=noms

    // ── État de jeu ───────────────────────────────────────────────────────────
    enum class State { PLAYING, BUY_PROMPT, GAMBLE_PROMPT, GAMEOVER };
    State state_ = State::PLAYING;

    int                     posJoueurs_[4] = {};
    int                     currentPlayer_ = 0;
    int                     lastDe1_ = 1, lastDe2_ = 1;
    bool                    hasDiceResult_  = false;
    std::deque<std::string> msgLog_;
    Joueur*                 gagnantFinal_  = nullptr;
    CasePropriete*          pendingBuy_    = nullptr;
    CasePropriete*          pendingGamble_ = nullptr;
    int                     miseSelectionnee_ = 100;

    // ── Cycle de vie interne ──────────────────────────────────────────────────
    void loadTextures();          // charge plateau + cellules + joueurs + dés
    void runSetupScreen();        // boucle bloquante jusqu'à "Démarrer"
    void buildGame();             // crée les Case* et Joueur* depuis cfg_
    void cleanupGame();           // supprime les objets, réinitialise l'état

    // ── Logique de tour ───────────────────────────────────────────────────────
    void doTurn();
    void addMsg(const std::string& s);
    int  countActifs() const;
    void nextActivePlayer();
    sf::Vector2f caseCenter(int idx) const;

    // ── Rendu (appelé depuis gamelooping / runSetupScreen) ────────────────────
    void renderBoard();        // plateau fond + textures cellules + voile + pions
    void renderPanel();        // panneau droit : dés, cartes joueurs, bouton
    void renderLog();          // bandeau log en bas du plateau
    void renderSetupScreen();  // écran de configuration
    void renderBuyPrompt();    // overlay achat de propriété
    void renderGamblePrompt(); // overlay choix de mise (gamble)
    void renderGameOver();     // overlay fin de partie

    // ── Utilitaires statiques ─────────────────────────────────────────────────
    static sf::Text makeText(const sf::Font& f, const std::string& s,
                             unsigned sz, sf::Color col, float x, float y);
    static void drawRect(sf::RenderWindow& w,
                         float x, float y, float bw, float bh,
                         sf::Color fill,
                         sf::Color outline = sf::Color::Transparent,
                         float thick = 0.f);
    static std::string condStr(Condition c);
    static sf::Color   condColor(Condition c);
};
