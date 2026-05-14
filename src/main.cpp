#define USE_SFML
#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <deque>
#include <algorithm>
#include <cstdlib>
#include <ctime>

#include "Joueur.hpp"
#include "Plateau.hpp"
#include "CaseDepart.hpp"
#include "CaseAleatoire.hpp"
#include "CasePropriete.hpp"
#include "CasePrison.hpp"
#include "CaseEvenement.hpp"
#include "Case.hpp"

// ── Layout constants ──────────────────────────────────────────────────────────
static const unsigned WIN_W   = 1300;
static const unsigned WIN_H   = 780;
static const unsigned BOARD_W = 900;
static const unsigned BOARD_H = 491; // 900/1408*768
static const unsigned PANEL_X = BOARD_W;
static const unsigned PANEL_W = WIN_W - BOARD_W; // 400
static const unsigned LOG_Y   = BOARD_H;
static const unsigned LOG_H   = WIN_H - BOARD_H; // 289

static const int NB_CASES = 12;

// Case centre positions as fraction of board image
static const float CASE_NX[NB_CASES] = {
    0.875f, 0.625f, 0.375f, 0.125f,
    0.125f, 0.125f, 0.125f,
    0.375f, 0.625f, 0.875f,
    0.875f, 0.875f
};
static const float CASE_NY[NB_CASES] = {
    0.875f, 0.875f, 0.875f, 0.875f,
    0.625f, 0.375f, 0.125f,
    0.125f, 0.125f, 0.125f,
    0.375f, 0.625f
};

// Player pion offsets so tokens on same case don't overlap
static const float PION_OX[4] = {-16.f, +16.f, -16.f, +16.f};
static const float PION_OY[4] = {-16.f, -16.f, +16.f, +16.f};

static const sf::Color PLAYER_COLORS[4] = {
    sf::Color(220, 60,  60),
    sf::Color(60,  120, 220),
    sf::Color(60,  200, 60),
    sf::Color(220, 180, 0)
};

enum class GameState { INIT, PLAYING, BUY_PROMPT, GAMEOVER };

// ── Helper: quick sf::Text builder ───────────────────────────────────────────
static sf::Text makeText(const sf::Font& font, const std::string& str,
                         unsigned size, sf::Color color,
                         float x, float y)
{
    sf::Text t;
    t.setFont(font);
    t.setString(str);
    t.setCharacterSize(size);
    t.setFillColor(color);
    t.setPosition(x, y);
    return t;
}

// ── Helper: draw rounded rect ─────────────────────────────────────────────────
static void drawRect(sf::RenderWindow& w, float x, float y, float bw, float bh,
                     sf::Color fill, sf::Color outline = sf::Color::Transparent,
                     float thick = 0.f)
{
    sf::RectangleShape r(sf::Vector2f(bw, bh));
    r.setPosition(x, y);
    r.setFillColor(fill);
    r.setOutlineColor(outline);
    r.setOutlineThickness(thick);
    w.draw(r);
}

// ── Helper: condition string ──────────────────────────────────────────────────
static std::string condStr(Condition c)
{
    if (c == Condition::FAILLITE)    return "FAILLITE";
    if (c == Condition::BANQUEROUTE) return "BANQ.";
    return "RICHE";
}

static sf::Color condColor(Condition c)
{
    if (c == Condition::FAILLITE)    return sf::Color(200, 60, 60);
    if (c == Condition::BANQUEROUTE) return sf::Color(220, 150, 0);
    return sf::Color(60, 200, 60);
}

// ─────────────────────────────────────────────────────────────────────────────
int main()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    sf::RenderWindow window(sf::VideoMode(WIN_W, WIN_H), "Gamble Style",
                            sf::Style::Close);
    window.setFramerateLimit(60);

    // ── Load font ─────────────────────────────────────────────────────────────
    sf::Font font;
    bool hasFont = font.loadFromFile(
        "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf");

    // ── Load textures ─────────────────────────────────────────────────────────
    sf::Texture texPlateau;
    if (!texPlateau.loadFromFile("assets/plateau.png")) {
        std::cerr << "Impossible de charger assets/plateau.png\n";
        return 1;
    }

    sf::Texture texJoueur[4];
    for (int i = 0; i < 4; ++i)
        texJoueur[i].loadFromFile("assets/joueur" + std::to_string(i + 1) + ".png");

    sf::Texture texDe[6];
    for (int i = 0; i < 6; ++i)
        texDe[i].loadFromFile("assets/de" + std::to_string(i + 1) + ".png");

    // ── Plateau sprite scaled to BOARD_W × BOARD_H ───────────────────────────
    sf::Sprite spritePlateau(texPlateau);
    spritePlateau.setScale(
        float(BOARD_W) / texPlateau.getSize().x,
        float(BOARD_H) / texPlateau.getSize().y);

    // ── Init-screen state ─────────────────────────────────────────────────────
    GameState state     = GameState::INIT;
    int       nbJoueurs = 2;
    std::string noms[4] = {"Joueur 1", "Joueur 2", "Joueur 3", "Joueur 4"};
    std::string dureeStr = "30";
    int activeField = -1; // -1=none, 0=duree, 1..4=noms[0..3]

    // ── Game objects (allocated on start) ─────────────────────────────────────
    Plateau*             plateau      = nullptr;
    std::vector<Joueur*> joueurs;
    std::vector<Case*>   cases;
    int  posJoueurs[4]  = {0, 0, 0, 0};
    int  currentPlayer  = 0;
    int  lastDe1 = 1, lastDe2 = 1;
    bool hasDiceResult  = false;
    std::deque<std::string> msgLog;
    Joueur*        gagnantFinal = nullptr;
    CasePropriete* pendingBuy   = nullptr;

    // ── Helpers ───────────────────────────────────────────────────────────────
    auto addMsg = [&](const std::string& s) {
        msgLog.push_front(s);
        if (msgLog.size() > 9) msgLog.pop_back();
    };

    auto casePixel = [&](int idx) -> sf::Vector2f {
        return { CASE_NX[idx] * BOARD_W, CASE_NY[idx] * BOARD_H };
    };

    // Count active (non-FAILLITE) players
    auto countActifs = [&]() {
        int n = 0;
        for (int i = 0; i < (int)joueurs.size(); ++i)
            if (joueurs[i]->conditionfinanciere() != Condition::FAILLITE) ++n;
        return n;
    };

    // Advance currentPlayer to next active
    auto nextPlayer = [&]() {
        if (joueurs.empty()) return;
        int tries = (int)joueurs.size();
        do {
            currentPlayer = (currentPlayer + 1) % (int)joueurs.size();
            --tries;
        } while (tries > 0 &&
                 joueurs[currentPlayer]->conditionfinanciere() == Condition::FAILLITE);
    };

    // ── startGame ─────────────────────────────────────────────────────────────
    auto startGame = [&]() {
        for (auto j : joueurs) delete j;
        joueurs.clear();
        for (auto c : cases) delete c;
        cases.clear();
        delete plateau;

        plateau = new Plateau(1500.0f);
        int dur = 0;
        try { dur = std::stoi(dureeStr); } catch (...) { dur = 30; }
        plateau->setTempsJeu(dur);

        for (int i = 0; i < nbJoueurs; ++i) {
            Joueur* j = new Joueur(noms[i], 1500);
            joueurs.push_back(j);
            plateau->ajouterJoueur(j);
        }

        cases.push_back(new CaseDepart(200, 0));
        cases.push_back(new CasePropriete(1, "Rue de la Paix",     200));
        cases.push_back(new CaseAleatoire(2));
        cases.push_back(new CasePropriete(3, "Av. des Fleurs",     150));
        cases.push_back(new CasePrison(4));
        cases.push_back(new CasePropriete(5, "Bd Royal",           300));
        cases.push_back(new CaseEvenement(6));
        cases.push_back(new CasePropriete(7, "Rue du Commerce",    120));
        cases.push_back(new CaseAleatoire(8));
        cases.push_back(new CasePropriete(9, "Place du Marché",    180));
        cases.push_back(new CaseEvenement(10));
        cases.push_back(new CasePropriete(11, "Chemin Vert",       100));

        // Wire all players into every CaseEvenement
        for (auto* cs : cases) {
            auto* ce = dynamic_cast<CaseEvenement*>(cs);
            if (ce) for (auto* j : joueurs) ce->ajouterJoueur(j);
            plateau->addCase(cs);
        }

        for (int i = 0; i < nbJoueurs; ++i) {
            posJoueurs[i] = 0;
            joueurs[i]->setCaseActuelle(cases[0]);
        }

        msgLog.clear();
        hasDiceResult  = false;
        currentPlayer  = 0;
        pendingBuy     = nullptr;
        gagnantFinal   = nullptr;

        // Roll to determine initial order
        plateau->ordre();
        joueurs.clear();
        for (int i = 0; i < nbJoueurs; ++i) joueurs.push_back(plateau->getJoueur(i));

        addMsg("Partie démarrée ! Tour de " + joueurs[0]->getNom());
        state = GameState::PLAYING;
    };

    // ── doTurn ────────────────────────────────────────────────────────────────
    auto doTurn = [&]() {
        Joueur* j = joueurs[currentPlayer];

        int de1  = (std::rand() % 6) + 1;
        int de2  = (std::rand() % 6) + 1;
        int roll = de1 + de2;
        lastDe1 = de1; lastDe2 = de2;
        hasDiceResult = true;

        int oldPos = posJoueurs[currentPlayer];
        int newPos = (oldPos + roll) % NB_CASES;
        posJoueurs[currentPlayer] = newPos;
        j->setCaseActuelle(cases[newPos]);

        // Bonus passage par DÉPART
        if (newPos < oldPos || (roll > 0 && newPos == 0)) {
            j->setCapital(j->getCapital() + 200);
            addMsg(j->getNom() + " passe par DÉPART  +200€");
        }

        std::ostringstream oss;
        oss << j->getNom() << " : " << de1 << "+" << de2 << "=" << roll
            << "  →  case " << newPos;
        addMsg(oss.str());

        cases[newPos]->setJoueurActif(j);

        CasePropriete* cp = dynamic_cast<CasePropriete*>(cases[newPos]);
        if (cp) {
            cp->action();
            if (cp->achatPropose()) {
                pendingBuy = cp;
                addMsg("Acheter \"" + cp->getName() + "\" pour " +
                       std::to_string(cp->getPrix()) + "€ ?");
                state = GameState::BUY_PROMPT;
                return;
            } else if (cp->minijeuLance()) {
                Joueur* prop = cp->getProprietaire();
                addMsg("Mini-jeu ! " + j->getNom() + " vs " +
                       (prop ? prop->getNom() : "?") +
                       "  →  " + j->getNom() + " " +
                       std::to_string(j->getCapital()) + "€");
            }
        } else {
            cases[newPos]->action();
            if (dynamic_cast<CaseDepart*>(cases[newPos]))
                addMsg(j->getNom() + " est sur DÉPART  +200€");
            else if (dynamic_cast<CasePrison*>(cases[newPos]))
                addMsg(j->getNom() + " est en Prison !");
            else if (dynamic_cast<CaseEvenement*>(cases[newPos]))
                addMsg("Mini-jeu tous joueurs !");
        }

        if (j->conditionfinanciere() == Condition::FAILLITE)
            addMsg(j->getNom() + " est en FAILLITE !");

        if (countActifs() <= 1) {
            gagnantFinal = plateau->fin();
            state = GameState::GAMEOVER;
            addMsg("FIN ! Gagnant : " +
                   (gagnantFinal ? gagnantFinal->getNom() : "?"));
            return;
        }

        nextPlayer();
        addMsg("→ Tour de " + joueurs[currentPlayer]->getNom());
    };

    // ─────────────────────────────────────────────────────────────────────────
    //  MAIN LOOP
    // ─────────────────────────────────────────────────────────────────────────
    while (window.isOpen())
    {
        sf::Event ev;
        while (window.pollEvent(ev))
        {
            if (ev.type == sf::Event::Closed) { window.close(); break; }

            // ── Keyboard text input (INIT) ──────────────────────────────────
            if (state == GameState::INIT && ev.type == sf::Event::TextEntered) {
                auto ch = ev.text.unicode;
                std::string* target = nullptr;
                if (activeField == 0) target = &dureeStr;
                else if (activeField >= 1 && activeField <= 4) target = &noms[activeField - 1];

                if (target) {
                    if (ch == 8 && !target->empty())          // backspace
                        target->pop_back();
                    else if (ch >= 32 && ch < 127 && target->size() < 20)
                        *target += static_cast<char>(ch);
                }
            }

            if (ev.type != sf::Event::MouseButtonPressed ||
                ev.mouseButton.button != sf::Mouse::Left) continue;

            sf::Vector2f mouse(float(ev.mouseButton.x), float(ev.mouseButton.y));

            // ── INIT screen clicks ──────────────────────────────────────────
            if (state == GameState::INIT)
            {
                // Nb players buttons  [2] [3] [4]
                for (int n = 2; n <= 4; ++n) {
                    float bx = 300.f + (n - 2) * 120.f;
                    sf::FloatRect r(bx, 240.f, 90.f, 42.f);
                    if (r.contains(mouse)) { nbJoueurs = n; activeField = -1; }
                }
                // Name fields
                for (int i = 0; i < nbJoueurs; ++i) {
                    float fy = 340.f + i * 60.f;
                    sf::FloatRect r(300.f, fy, 380.f, 40.f);
                    if (r.contains(mouse)) activeField = i + 1;
                }
                // Duration field
                {
                    sf::FloatRect r(300.f, 340.f + nbJoueurs * 60.f + 20.f, 180.f, 40.f);
                    if (r.contains(mouse)) activeField = 0;
                }
                // Start button
                {
                    float sy = 340.f + nbJoueurs * 60.f + 80.f;
                    sf::FloatRect r(380.f, sy, 200.f, 50.f);
                    if (r.contains(mouse)) startGame();
                }
            }

            // ── BUY_PROMPT clicks ───────────────────────────────────────────
            else if (state == GameState::BUY_PROMPT && pendingBuy)
            {
                // [Oui] button
                sf::FloatRect rOui(420.f, 460.f, 130.f, 48.f);
                // [Non] button
                sf::FloatRect rNon(600.f, 460.f, 130.f, 48.f);

                if (rOui.contains(mouse)) {
                    Joueur* j = joueurs[currentPlayer];
                    if (j->getCapital() >= pendingBuy->getPrix()) {
                        j->acheter(pendingBuy);
                        addMsg(j->getNom() + " achète \"" + pendingBuy->getName() +
                               "\"  -" + std::to_string(pendingBuy->getPrix()) + "€");
                    } else {
                        addMsg(j->getNom() + " n'a pas assez de capital !");
                    }
                    pendingBuy = nullptr;
                    if (countActifs() <= 1) {
                        gagnantFinal = plateau->fin();
                        state = GameState::GAMEOVER;
                    } else {
                        nextPlayer();
                        addMsg("→ Tour de " + joueurs[currentPlayer]->getNom());
                        state = GameState::PLAYING;
                    }
                } else if (rNon.contains(mouse)) {
                    addMsg(joueurs[currentPlayer]->getNom() + " refuse d'acheter.");
                    pendingBuy = nullptr;
                    if (countActifs() <= 1) {
                        gagnantFinal = plateau->fin();
                        state = GameState::GAMEOVER;
                    } else {
                        nextPlayer();
                        addMsg("→ Tour de " + joueurs[currentPlayer]->getNom());
                        state = GameState::PLAYING;
                    }
                }
            }

            // ── PLAYING screen clicks ───────────────────────────────────────
            else if (state == GameState::PLAYING)
            {
                // "Lancer le dé" button  (right panel, bottom)
                sf::FloatRect btnRect(float(PANEL_X) + 50.f, float(WIN_H) - 80.f,
                                      300.f, 58.f);
                if (btnRect.contains(mouse)) doTurn();
            }

            // ── GAMEOVER screen ─────────────────────────────────────────────
            else if (state == GameState::GAMEOVER)
            {
                // "Nouvelle partie" button
                sf::FloatRect r(WIN_W / 2.f - 140.f, 520.f, 280.f, 54.f);
                if (r.contains(mouse)) state = GameState::INIT;
            }
        }

        // ─────────────────────────────────────────────────────────────────────
        //  RENDERING
        // ─────────────────────────────────────────────────────────────────────
        window.clear(sf::Color(25, 25, 35));

        // ════════════════════════════════════════════════════════════════════
        //  INIT SCREEN
        // ════════════════════════════════════════════════════════════════════
        if (state == GameState::INIT)
        {
            // Background
            drawRect(window, 0, 0, WIN_W, WIN_H, sf::Color(25, 25, 35));

            // Title
            if (hasFont) {
                auto title = makeText(font, "GAMBLE  STYLE", 58,
                                      sf::Color(220, 180, 0), 0, 60);
                title.setStyle(sf::Text::Bold);
                sf::FloatRect tb = title.getLocalBounds();
                title.setPosition((WIN_W - tb.width) / 2.f, 60.f);
                window.draw(title);

                auto sub = makeText(font, "Nouvelle partie", 22,
                                    sf::Color(180, 180, 180), 0, 140);
                sf::FloatRect sb = sub.getLocalBounds();
                sub.setPosition((WIN_W - sb.width) / 2.f, 140.f);
                window.draw(sub);

                // Separator line
                drawRect(window, 200, 185, 900, 2, sf::Color(100, 100, 100));

                // Nb players
                window.draw(makeText(font, "Nombre de joueurs :", 20,
                                     sf::Color(220, 220, 220), 160, 250));
                for (int n = 2; n <= 4; ++n) {
                    float bx = 300.f + (n - 2) * 120.f;
                    bool sel = (n == nbJoueurs);
                    drawRect(window, bx, 240.f, 90.f, 42.f,
                             sel ? sf::Color(220, 180, 0) : sf::Color(60, 60, 80),
                             sf::Color(150, 150, 150), 1.5f);
                    auto nt = makeText(font, std::to_string(n) + " joueurs", 16,
                                       sel ? sf::Color(20, 20, 20) : sf::Color(220, 220, 220),
                                       bx + 8.f, 250.f);
                    window.draw(nt);
                }

                // Name fields
                window.draw(makeText(font, "Noms des joueurs :", 20,
                                     sf::Color(220, 220, 220), 160, 315));
                for (int i = 0; i < nbJoueurs; ++i) {
                    float fy = 340.f + i * 60.f;
                    bool act = (activeField == i + 1);
                    drawRect(window, 300.f, fy, 380.f, 40.f,
                             sf::Color(40, 40, 55),
                             act ? sf::Color(220, 180, 0) : sf::Color(100, 100, 120),
                             act ? 2.f : 1.f);
                    // Colour dot
                    sf::CircleShape dot(8.f);
                    dot.setFillColor(PLAYER_COLORS[i]);
                    dot.setPosition(268.f, fy + 12.f);
                    window.draw(dot);
                    std::string display = noms[i] + (act ? "|" : "");
                    window.draw(makeText(font, display, 18,
                                        sf::Color(240, 240, 240), 310.f, fy + 8.f));
                }

                // Duration field
                float dy = 340.f + nbJoueurs * 60.f + 20.f;
                bool actDur = (activeField == 0);
                window.draw(makeText(font, "Durée (min) :", 20,
                                     sf::Color(220, 220, 220), 160, dy + 6.f));
                drawRect(window, 300.f, dy, 180.f, 40.f,
                         sf::Color(40, 40, 55),
                         actDur ? sf::Color(220, 180, 0) : sf::Color(100, 100, 120),
                         actDur ? 2.f : 1.f);
                window.draw(makeText(font, dureeStr + (actDur ? "|" : ""),
                                     18, sf::Color(240, 240, 240), 310.f, dy + 8.f));

                // Start button
                float sy = 340.f + nbJoueurs * 60.f + 80.f;
                drawRect(window, 380.f, sy, 200.f, 50.f,
                         sf::Color(220, 180, 0), sf::Color::Black, 2.f);
                window.draw(makeText(font, "Démarrer", 22,
                                     sf::Color(20, 20, 20), 430.f, sy + 10.f));
            }
        }

        // ════════════════════════════════════════════════════════════════════
        //  PLAYING + BUY_PROMPT
        // ════════════════════════════════════════════════════════════════════
        else if (state == GameState::PLAYING || state == GameState::BUY_PROMPT)
        {
            // ── Plateau ───────────────────────────────────────────────────────
            window.draw(spritePlateau);

            // Highlight current player's case
            if (!joueurs.empty()) {
                sf::Vector2f cc = casePixel(posJoueurs[currentPlayer]);
                sf::RectangleShape hl(sf::Vector2f(108.f, 108.f));
                hl.setFillColor(sf::Color(255, 255, 0, 50));
                hl.setOutlineColor(sf::Color(255, 220, 0, 200));
                hl.setOutlineThickness(3.f);
                hl.setPosition(cc.x - 54.f, cc.y - 54.f);
                window.draw(hl);
            }

            // Draw player pions (joueur sprites scaled to 42×48)
            for (int i = 0; i < (int)joueurs.size(); ++i) {
                if (joueurs[i]->conditionfinanciere() == Condition::FAILLITE) continue;
                sf::Vector2f cp = casePixel(posJoueurs[i]);
                float px = cp.x + PION_OX[i];
                float py = cp.y + PION_OY[i];

                if (texJoueur[i].getSize().x > 0) {
                    sf::Sprite sp(texJoueur[i]);
                    sp.setScale(42.f / texJoueur[i].getSize().x,
                                48.f / texJoueur[i].getSize().y);
                    sp.setPosition(px - 21.f, py - 24.f);
                    window.draw(sp);
                } else {
                    sf::CircleShape c(16.f);
                    c.setFillColor(PLAYER_COLORS[i]);
                    c.setOutlineColor(sf::Color::White);
                    c.setOutlineThickness(2.f);
                    c.setPosition(px - 16.f, py - 16.f);
                    window.draw(c);
                }
            }

            // ── Message log (below plateau) ───────────────────────────────────
            drawRect(window, 0, LOG_Y, BOARD_W, LOG_H, sf::Color(15, 15, 25, 230));
            drawRect(window, 0, LOG_Y, BOARD_W, 2, sf::Color(80, 80, 100));
            if (hasFont) {
                for (int i = 0; i < (int)msgLog.size(); ++i) {
                    float fy = float(LOG_Y) + 10.f + i * 30.f;
                    sf::Color col = (i == 0) ? sf::Color(240, 240, 100)
                                             : sf::Color(180, 180, 180);
                    window.draw(makeText(font, msgLog[i], 16, col, 12.f, fy));
                }
            }

            // ── Right panel background ────────────────────────────────────────
            drawRect(window, PANEL_X, 0, PANEL_W, WIN_H, sf::Color(18, 18, 28));
            drawRect(window, PANEL_X, 0, 2, WIN_H, sf::Color(80, 80, 100));

            if (hasFont)
            {
                // ── Turn label ────────────────────────────────────────────────
                std::string turnStr = joueurs.empty() ? "" :
                    "Tour de " + joueurs[currentPlayer]->getNom();
                window.draw(makeText(font, turnStr, 18,
                                     PLAYER_COLORS[currentPlayer],
                                     float(PANEL_X) + 20.f, 12.f));

                // ── Dice display ──────────────────────────────────────────────
                float diceY = 45.f;
                float diceSize = 96.f;
                float dx1 = float(PANEL_X) + 20.f;
                float dx2 = float(PANEL_X) + 140.f;

                if (hasDiceResult) {
                    // Die 1
                    sf::Sprite s1(texDe[lastDe1 - 1]);
                    s1.setScale(diceSize / texDe[lastDe1 - 1].getSize().x,
                                diceSize / texDe[lastDe1 - 1].getSize().y);
                    s1.setPosition(dx1, diceY);
                    window.draw(s1);
                    // Die 2
                    sf::Sprite s2(texDe[lastDe2 - 1]);
                    s2.setScale(diceSize / texDe[lastDe2 - 1].getSize().x,
                                diceSize / texDe[lastDe2 - 1].getSize().y);
                    s2.setPosition(dx2, diceY);
                    window.draw(s2);
                    // "+" and "=" labels
                    window.draw(makeText(font, "+", 28, sf::Color(200, 200, 200),
                                         dx1 + diceSize + 8.f, diceY + 34.f));
                    window.draw(makeText(font,
                                         "= " + std::to_string(lastDe1 + lastDe2),
                                         22, sf::Color(220, 220, 100),
                                         float(PANEL_X) + 260.f, diceY + 36.f));
                } else {
                    drawRect(window, dx1, diceY, diceSize, diceSize,
                             sf::Color(40, 40, 55), sf::Color(80, 80, 100), 2.f);
                    drawRect(window, dx2, diceY, diceSize, diceSize,
                             sf::Color(40, 40, 55), sf::Color(80, 80, 100), 2.f);
                    window.draw(makeText(font, "?", 48, sf::Color(100, 100, 120),
                                         dx1 + 34.f, diceY + 20.f));
                    window.draw(makeText(font, "?", 48, sf::Color(100, 100, 120),
                                         dx2 + 34.f, diceY + 20.f));
                }

                // Separator
                drawRect(window, float(PANEL_X) + 10, 155, PANEL_W - 20, 1,
                         sf::Color(70, 70, 90));

                // ── Player cards ──────────────────────────────────────────────
                float cardY = 165.f;
                float cardH = std::min(115.f, float(WIN_H - 90 - 165) / joueurs.size());
                float cardW = float(PANEL_W) - 20.f;

                for (int i = 0; i < (int)joueurs.size(); ++i) {
                    Joueur* j   = joueurs[i];
                    Condition c = j->conditionfinanciere();
                    bool isCur  = (i == currentPlayer);

                    sf::Color bgCol = isCur ? sf::Color(45, 45, 65)
                                            : sf::Color(28, 28, 40);
                    sf::Color brdCol = isCur ? PLAYER_COLORS[i]
                                             : sf::Color(60, 60, 80);
                    float brdThick = isCur ? 2.f : 1.f;

                    drawRect(window, float(PANEL_X) + 10.f, cardY,
                             cardW, cardH - 4.f,
                             bgCol, brdCol, brdThick);

                    // Colour strip on left
                    drawRect(window, float(PANEL_X) + 10.f, cardY,
                             6.f, cardH - 4.f, PLAYER_COLORS[i]);

                    // Player sprite thumbnail (25×28)
                    if (texJoueur[i].getSize().x > 0) {
                        sf::Sprite th(texJoueur[i]);
                        th.setScale(25.f / texJoueur[i].getSize().x,
                                    28.f / texJoueur[i].getSize().y);
                        th.setPosition(float(PANEL_X) + 20.f, cardY + 6.f);
                        window.draw(th);
                    }

                    float tx = float(PANEL_X) + 52.f;
                    sf::Color nameCol = (c == Condition::FAILLITE)
                                        ? sf::Color(120, 120, 120)
                                        : sf::Color(230, 230, 230);
                    window.draw(makeText(font, j->getNom(), 16, nameCol, tx, cardY + 5.f));
                    window.draw(makeText(font,
                        std::to_string(j->getCapital()) + " €",
                        15, sf::Color(100, 220, 100), tx, cardY + 26.f));
                    window.draw(makeText(font,
                        condStr(c), 13, condColor(c), tx, cardY + 47.f));
                    window.draw(makeText(font,
                        std::to_string(j->getNbProprietes()) + " prop.",
                        13, sf::Color(150, 180, 220), tx + 100.f, cardY + 47.f));

                    cardY += cardH;
                }

                // ── "Lancer le dé" button ─────────────────────────────────────
                if (state == GameState::PLAYING) {
                    drawRect(window, float(PANEL_X) + 50.f, float(WIN_H) - 80.f,
                             300.f, 58.f,
                             sf::Color(180, 140, 0), sf::Color::Black, 2.f);
                    window.draw(makeText(font, "Lancer le dé", 22,
                                         sf::Color(20, 20, 20),
                                         float(PANEL_X) + 80.f, float(WIN_H) - 66.f));
                }
            }

            // ── BUY_PROMPT overlay ────────────────────────────────────────────
            if (state == GameState::BUY_PROMPT && pendingBuy && hasFont)
            {
                // Dim overlay
                sf::RectangleShape dim{sf::Vector2f(float(WIN_W), float(WIN_H))};
                dim.setFillColor(sf::Color(0, 0, 0, 160));
                window.draw(dim);

                // Dialog box
                drawRect(window, 300.f, 310.f, 560.f, 270.f,
                         sf::Color(30, 30, 45), sf::Color(220, 180, 0), 2.f);

                window.draw(makeText(font, "Achat de propriété", 24,
                                     sf::Color(220, 180, 0), 390.f, 330.f));
                window.draw(makeText(font,
                    "\"" + pendingBuy->getName() + "\"",
                    20, sf::Color(240, 240, 240), 340.f, 375.f));
                window.draw(makeText(font,
                    "Prix : " + std::to_string(pendingBuy->getPrix()) + " €",
                    18, sf::Color(100, 220, 100), 340.f, 408.f));
                window.draw(makeText(font,
                    "Votre capital : " +
                    std::to_string(joueurs[currentPlayer]->getCapital()) + " €",
                    16, sf::Color(180, 180, 180), 340.f, 436.f));

                // [Oui] button
                drawRect(window, 420.f, 460.f, 130.f, 48.f,
                         sf::Color(60, 160, 60), sf::Color::Black, 2.f);
                window.draw(makeText(font, "Acheter", 20,
                                     sf::Color::White, 442.f, 470.f));
                // [Non] button
                drawRect(window, 600.f, 460.f, 130.f, 48.f,
                         sf::Color(160, 60, 60), sf::Color::Black, 2.f);
                window.draw(makeText(font, "Passer", 20,
                                     sf::Color::White, 624.f, 470.f));
            }
        }

        // ════════════════════════════════════════════════════════════════════
        //  GAMEOVER SCREEN
        // ════════════════════════════════════════════════════════════════════
        else if (state == GameState::GAMEOVER)
        {
            // Show board dimmed as background
            window.draw(spritePlateau);

            sf::RectangleShape dim{sf::Vector2f(float(WIN_W), float(WIN_H))};
            dim.setFillColor(sf::Color(0, 0, 0, 180));
            window.draw(dim);

            if (hasFont) {
                drawRect(window, WIN_W / 2.f - 280.f, 180.f, 560.f, 390.f,
                         sf::Color(25, 25, 40), sf::Color(220, 180, 0), 3.f);

                auto txt = makeText(font, "FIN DE PARTIE", 42,
                                    sf::Color(220, 180, 0), 0, 210.f);
                sf::FloatRect tb = txt.getLocalBounds();
                txt.setPosition((WIN_W - tb.width) / 2.f, 210.f);
                window.draw(txt);

                std::string winName = gagnantFinal ? gagnantFinal->getNom() : "?";
                auto wt = makeText(font, "Gagnant : " + winName, 28,
                                   sf::Color(100, 240, 100), 0, 300.f);
                sf::FloatRect wb = wt.getLocalBounds();
                wt.setPosition((WIN_W - wb.width) / 2.f, 300.f);
                window.draw(wt);

                // Scores
                if (plateau) {
                    float sy = 355.f;
                    for (int i = 0; i < (int)joueurs.size(); ++i) {
                        Joueur* j = joueurs[i];
                        int score = j->getCapital() + j->getValPropriete();
                        std::ostringstream oss;
                        oss << j->getNom() << "  :  " << score << " €";
                        auto st = makeText(font, oss.str(), 18,
                                           PLAYER_COLORS[i], 0, sy);
                        sf::FloatRect stb = st.getLocalBounds();
                        st.setPosition((WIN_W - stb.width) / 2.f, sy);
                        window.draw(st);
                        sy += 34.f;
                    }
                }

                // "Nouvelle partie" button
                drawRect(window, WIN_W / 2.f - 140.f, 520.f, 280.f, 54.f,
                         sf::Color(220, 180, 0), sf::Color::Black, 2.f);
                auto bt = makeText(font, "Nouvelle partie", 22,
                                   sf::Color(20, 20, 20), 0, 530.f);
                sf::FloatRect btb = bt.getLocalBounds();
                bt.setPosition((WIN_W - btb.width) / 2.f, 530.f);
                window.draw(bt);
            }
        }

        window.display();
    }

    // ── Cleanup ───────────────────────────────────────────────────────────────
    for (auto j : joueurs) delete j;
    for (auto c : cases)   delete c;
    delete plateau;
    return 0;
}
