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
class CaseEvenement;
class CarteSortiePrison;
class CarteDoubleGain;

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

    void initialization();
    void gamelooping();

private:
    // ── Fenêtre & rendu ───────────────────────────────────────────────────────
    sf::RenderWindow window_;
    sf::View         gameView_;     // maps design coords to fullscreen
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

    // ── Mise en page ──────────────────────────────────────────────────────────
    static constexpr unsigned BOARD_SIZE = 670u;
    static constexpr unsigned PANEL_W    = 420u;
    static constexpr unsigned LOG_H      = 130u;
    static constexpr unsigned WIN_W      = BOARD_SIZE + PANEL_W;  // 1090
    static constexpr unsigned WIN_H      = BOARD_SIZE + LOG_H;    // 800
    float cellPx_ = 0.f;

    // ── Écran de configuration ────────────────────────────────────────────────
    int         nbJoueurs_   = 2;
    std::string noms_[4]     = {"Joueur 1", "Joueur 2", "Joueur 3", "Joueur 4"};
    std::string dureeStr_    = "30";
    int         activeField_ = -1;

    // ── État de jeu ───────────────────────────────────────────────────────────
    enum class State { PLAYING, MOVING, BUY_PROMPT, GAMBLE_PROMPT,
                       USE_SORTIE_PRISON_PROMPT, USE_DOUBLE_GAIN_PROMPT,
                       CARTE_DRAWN_PROMPT, GAMEOVER };
    State state_ = State::PLAYING;

    int                     posJoueurs_[4]    = {};
    int                     currentPlayer_    = 0;
    int                     lastDe1_ = 1,     lastDe2_ = 1;
    bool                    hasDiceResult_    = false;
    std::deque<std::string> msgLog_;
    Joueur*                 gagnantFinal_     = nullptr;
    CasePropriete*          pendingBuy_             = nullptr;
    CasePropriete*          pendingGamble_          = nullptr;
    int                     miseSelectionnee_       = 100;
    int                     prisonIdx_              = -1;
    CarteSortiePrison*      pendingSortiePrison_    = nullptr;
    CarteDoubleGain*        pendingDoubleGainCard_  = nullptr;
    bool                    pendingDoubleGain_      = false;

    // ── Carte piochée (CaseAleatoire) ─────────────────────────────────────────
    sf::Texture             carteTexture_;
    bool                    hasCarteTexture_    = false;
    std::string             carteTitre_;
    std::string             carteExplication_;  // lignes séparées par '\n'

    // ── Doubles & prison ─────────────────────────────────────────────────────
    int       doubleStreak_[4] = {};  // streak de doubles par joueur
    bool      autoRoll_        = false;
    sf::Clock autoRollClock_;
    sf::Clock actionClock_;           // délai 1 s entre mouvement et résolution

    // ── Cycle de vie interne ──────────────────────────────────────────────────
    void loadTextures();
    void runSetupScreen();
    void buildGame();
    void cleanupGame();

    // ── Logique de tour ───────────────────────────────────────────────────────
    void doTurn();
    void resolveAction();       // résolution différée de l'action de la case courante
    void advanceTurn();         // passe au joueur suivant ou laisse rejouer sur double
    void addMsg(const std::string& s);
    int  countActifs() const;
    void nextActivePlayer();
    sf::Vector2f caseCenter(int idx) const;

    // ── Rendu ─────────────────────────────────────────────────────────────────
    void renderBoard();
    void renderPanel();
    void renderLog();
    void renderSetupScreen();
    void renderBuyPrompt();
    void renderGamblePrompt();
    void renderUseSortiePrisonPrompt();
    void renderUseDoubleGainPrompt();
    void renderCarteDrawnPrompt();
    void renderGameOver();

    // Prépare les données d'affichage pour la carte piochée
    void prepareCarteDisplay(Carte* card, Joueur* titulaire,
                             int capDelta, int posBefore, int posAfter);

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
