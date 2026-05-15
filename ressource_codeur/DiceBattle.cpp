// DiceBattle — Mini-jeu externe : bataille de dés
//
// Usage : ./DiceBattle "Joueur1" "Joueur2" ["Joueur3" ["Joueur4"]]
// Sortie stdout : nom du gagnant suivi d'un saut de ligne
//
// Disposition :
//   Panneau gauche  (~220 px) : tous les joueurs (sprite + nom + score après jeu)
//   Zone centrale (reste)     : deux dés animés + nom du joueur courant
//
// Animation du joueur courant : pulsation sinusoïdale ×1.3 max, 1 cycle/s.

#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>

// ── Constantes ────────────────────────────────────────────────────────────────
static const unsigned WIN_W  = 900u;
static const unsigned WIN_H  = 620u;
static const float    LEFT_W = 220.f;   // largeur panneau joueurs
static const float    BTN_H  = 54.f;    // hauteur bouton bas

static const sf::Color PLAYER_COLORS[4] = {
    sf::Color(220,  60,  60),
    sf::Color( 60, 120, 220),
    sf::Color( 60, 200,  60),
    sf::Color(220, 180,   0)
};

// ── Helpers ───────────────────────────────────────────────────────────────────
static sf::Text makeText(const sf::Font& f, const std::string& s, unsigned sz,
                          sf::Color col, float x, float y)
{
    sf::Text t;
    t.setFont(f);
    t.setString(sf::String::fromUtf8(s.begin(), s.end()));
    t.setCharacterSize(sz);
    t.setFillColor(col);
    t.setPosition(x, y);
    return t;
}

static void drawRect(sf::RenderWindow& w, float x, float y, float bw, float bh,
                     sf::Color fill,
                     sf::Color outline = sf::Color::Transparent, float thick = 0.f)
{
    sf::RectangleShape r(sf::Vector2f(bw, bh));
    r.setPosition(x, y);
    r.setFillColor(fill);
    r.setOutlineColor(outline);
    r.setOutlineThickness(thick);
    w.draw(r);
}

// ── États ─────────────────────────────────────────────────────────────────────
enum class Phase { ROLLING, FROZEN, RESULTS };

int main(int argc, char* argv[])
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // ── Noms des joueurs ──────────────────────────────────────────────────────
    std::vector<std::string> noms;
    for (int i = 1; i < argc && i <= 4; ++i)
        noms.push_back(argv[i]);
    if (noms.size() < 2)
        noms = {"Joueur 1", "Joueur 2"};
    const int N = static_cast<int>(noms.size());

    // ── Assets ────────────────────────────────────────────────────────────────
    sf::Font font;
    bool hasFont = font.loadFromFile(
        "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf");

    sf::Texture texDe[6];
    for (int i = 0; i < 6; ++i)
        texDe[i].loadFromFile("assets/de" + std::to_string(i + 1) + ".png");

    sf::Texture texJoueur[4];
    for (int i = 0; i < 4; ++i)
        texJoueur[i].loadFromFile("assets/joueur" + std::to_string(i + 1) + ".png");

    // ── Fenêtre borderless fullscreen ────────────────────────────────────────
    // On évite sf::Style::Fullscreen (exclusif, problématique en sous-processus)
    // et on utilise une fenêtre sans bordure à la taille du bureau.
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktop, "Bataille de Des", sf::Style::None);
    window.setPosition(sf::Vector2i(0, 0));
    window.setFramerateLimit(60);
    window.requestFocus();

    // Vue : espace de conception WIN_W×WIN_H letterboxé sur le bureau
    float sc  = std::min(float(desktop.width)  / float(WIN_W),
                         float(desktop.height) / float(WIN_H));
    float vpW = float(WIN_W) * sc / float(desktop.width);
    float vpH = float(WIN_H) * sc / float(desktop.height);
    sf::View gameView(sf::FloatRect(0.f, 0.f, float(WIN_W), float(WIN_H)));
    gameView.setViewport(sf::FloatRect(
        (1.f - vpW) * 0.5f, (1.f - vpH) * 0.5f, vpW, vpH));
    window.setView(gameView);

    // ── État ──────────────────────────────────────────────────────────────────
    Phase phase     = Phase::ROLLING;
    int   curPlayer = 0;
    int   animD1 = 1, animD2 = 1;
    int   winnerIdx = -1;

    std::vector<int>  d1s(N, 0), d2s(N, 0);
    std::vector<bool> played(N, false);

    sf::Clock totalClock;   // durée depuis début animation joueur courant
    sf::Clock frameClock;   // cadence dés (10 Hz)
    sf::Clock pulseClock;   // pulsation sprite joueur actif

    auto startRolling = [&]() {
        phase  = Phase::ROLLING;
        animD1 = (std::rand() % 6) + 1;
        animD2 = (std::rand() % 6) + 1;
        totalClock.restart();
        frameClock.restart();
        pulseClock.restart();
    };
    startRolling();

    // ── Mise en page ──────────────────────────────────────────────────────────
    const float slotH  = float(WIN_H - BTN_H) / N;
    const float centCX = LEFT_W + (WIN_W - LEFT_W) / 2.f;   // centre zone dés

    // ── Boucle principale ─────────────────────────────────────────────────────
    while (window.isOpen())
    {
        // Mise à jour animation dés (10 Hz) + gel après 3 s
        if (phase == Phase::ROLLING) {
            if (frameClock.getElapsedTime().asMilliseconds() > 100) {
                animD1 = (std::rand() % 6) + 1;
                animD2 = (std::rand() % 6) + 1;
                frameClock.restart();
            }
            if (totalClock.getElapsedTime().asSeconds() >= 3.f) {
                d1s[curPlayer]    = animD1;
                d2s[curPlayer]    = animD2;
                played[curPlayer] = true;
                phase = Phase::FROZEN;
            }
        }

        // ── Avance l'état (souris ou clavier) ────────────────────────────────
        auto doAdvance = [&]() {
            if (phase == Phase::FROZEN) {
                ++curPlayer;
                if (curPlayer >= N) {
                    winnerIdx = 0;
                    int maxScore = d1s[0] + d2s[0];
                    for (int i = 1; i < N; ++i) {
                        int s = d1s[i] + d2s[i];
                        if (s > maxScore) { maxScore = s; winnerIdx = i; }
                    }
                    phase = Phase::RESULTS;
                } else {
                    startRolling();
                }
            } else if (phase == Phase::RESULTS) {
                std::cout << noms[winnerIdx] << std::endl;
                window.close();
            }
        };

        sf::Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) {
                if (winnerIdx >= 0) std::cout << noms[winnerIdx] << std::endl;
                return 0;
            }

            // ── Clavier ───────────────────────────────────────────────────────
            if (ev.type == sf::Event::KeyPressed) {
                if (ev.key.code == sf::Keyboard::Escape) {
                    if (winnerIdx >= 0) std::cout << noms[winnerIdx] << std::endl;
                    return 0;
                }
                // Espace ou Entrée : avance (alternative fiable au bouton souris)
                if (ev.key.code == sf::Keyboard::Space ||
                    ev.key.code == sf::Keyboard::Return) {
                    doAdvance();
                }
                continue;
            }

            // ── Souris ────────────────────────────────────────────────────────
            if (ev.type != sf::Event::MouseButtonPressed ||
                ev.mouseButton.button != sf::Mouse::Left) continue;

            sf::Vector2f mouse = window.mapPixelToCoords(
                sf::Vector2i(ev.mouseButton.x, ev.mouseButton.y), gameView);
            sf::FloatRect btnRect(centCX - 90.f, WIN_H - BTN_H + 3.f, 180.f, BTN_H - 6.f);

            if (btnRect.contains(mouse))
                doAdvance();
        }

        // ── Rendu ─────────────────────────────────────────────────────────────
        window.clear(sf::Color(18, 18, 28));
        window.setView(gameView);   // toujours activer la vue avant de dessiner
        if (!hasFont) { window.display(); continue; }

        float pulseT = pulseClock.getElapsedTime().asSeconds();

        // ── Panneau gauche : tous les joueurs ──────────────────────────────────
        drawRect(window, 0, 0, LEFT_W, float(WIN_H), sf::Color(22, 22, 38));
        drawRect(window, LEFT_W - 2, 0, 2, float(WIN_H), sf::Color(55, 55, 85));

        for (int i = 0; i < N; ++i) {
            float sy  = i * slotH;
            sf::Color pc = PLAYER_COLORS[i];
            bool isCurrent = (i == curPlayer && phase != Phase::RESULTS);

            // Fond de slot teinté pour le joueur actif
            if (isCurrent) {
                drawRect(window, 0, sy, LEFT_W - 2, slotH,
                         sf::Color(pc.r / 5, pc.g / 5, pc.b / 5, 220));
                drawRect(window, 0, sy, 5, slotH, pc);
            }

            // Pulsation sinusoïdale (1 cycle/s) : échelle entre 1.0 et 1.3
            float scale = 1.f;
            if (isCurrent && phase == Phase::ROLLING)
                scale = 1.f + 0.3f * std::abs(std::sin(3.14159f * pulseT));

            // Sprite joueur centré dans le slot
            const float baseH = 62.f;
            float h  = baseH * scale;
            float cx = LEFT_W / 2.f;
            float cy = sy + slotH * 0.42f;

            if (texJoueur[i].getSize().x > 0) {
                float ratio = float(texJoueur[i].getSize().x) / texJoueur[i].getSize().y;
                float w = h * ratio;
                sf::Sprite av(texJoueur[i]);
                av.setScale(w / texJoueur[i].getSize().x, h / texJoueur[i].getSize().y);
                av.setPosition(cx - w / 2.f, cy - h / 2.f);
                // Halo coloré autour du sprite pour le joueur actif
                if (isCurrent) {
                    sf::RectangleShape glow(sf::Vector2f(w + 10, h + 10));
                    glow.setFillColor(sf::Color(pc.r, pc.g, pc.b, 50));
                    glow.setOutlineColor(sf::Color(pc.r, pc.g, pc.b, 210));
                    glow.setOutlineThickness(2.5f);
                    glow.setPosition(cx - w / 2.f - 5, cy - h / 2.f - 5);
                    window.draw(glow);
                }
                window.draw(av);
            } else {
                float r = h * 0.38f;
                sf::CircleShape circ(r);
                circ.setFillColor(pc);
                if (isCurrent) {
                    circ.setOutlineColor(sf::Color::White);
                    circ.setOutlineThickness(2.f);
                }
                circ.setPosition(cx - r, cy - r);
                window.draw(circ);
            }

            // Nom
            sf::Color nameCol = isCurrent ? sf::Color::White : sf::Color(150, 150, 150);
            sf::Text nt = makeText(font, noms[i], isCurrent ? 14u : 12u,
                                   nameCol, 0, 0);
            nt.setPosition(cx - nt.getLocalBounds().width / 2.f, sy + slotH * 0.76f);
            window.draw(nt);

            // Score (affiché une fois le joueur passé)
            if (played[i]) {
                int  tot = d1s[i] + d2s[i];
                bool win = (phase == Phase::RESULTS && i == winnerIdx);
                std::string sc = std::to_string(d1s[i]) + "+"
                               + std::to_string(d2s[i]) + "="
                               + std::to_string(tot)
                               + (win ? " ♛" : "");
                sf::Color sc_col = win ? sf::Color(80, 230, 80) : pc;
                sf::Text st = makeText(font, sc, win ? 14u : 12u, sc_col, 0, 0);
                st.setPosition(cx - st.getLocalBounds().width / 2.f, sy + slotH * 0.90f);
                window.draw(st);
            }

            // Séparateur
            if (i < N - 1)
                drawRect(window, 8, (i + 1) * slotH - 1, LEFT_W - 16, 1,
                         sf::Color(48, 48, 72));
        }

        // ── Zone centrale : dés + label ────────────────────────────────────────
        if (phase != Phase::RESULTS) {
            // Nom du joueur courant
            sf::Text lt = makeText(font, noms[curPlayer] + " lance les des...",
                                   21, PLAYER_COLORS[curPlayer], 0, 22);
            lt.setPosition(centCX - lt.getLocalBounds().width / 2.f, 22);
            window.draw(lt);

            // Deux grands dés centrés
            const float diceSz = 138.f;
            const float diceY  = WIN_H / 2.f - diceSz / 2.f - 20.f;
            int f1 = (phase == Phase::ROLLING) ? animD1 : d1s[curPlayer];
            int f2 = (phase == Phase::ROLLING) ? animD2 : d2s[curPlayer];

            for (int d = 0; d < 2; ++d) {
                int face = (d == 0 ? f1 : f2) - 1;
                float dx = centCX - diceSz - 12.f + d * (diceSz + 24.f);
                if (texDe[face].getSize().x > 0) {
                    sf::Sprite sp(texDe[face]);
                    sp.setScale(diceSz / texDe[face].getSize().x,
                                diceSz / texDe[face].getSize().y);
                    sp.setPosition(dx, diceY);
                    window.draw(sp);
                } else {
                    drawRect(window, dx, diceY, diceSz, diceSz,
                             sf::Color(38, 38, 58), sf::Color(90, 90, 120), 2.f);
                    window.draw(makeText(font, std::to_string(d == 0 ? f1 : f2),
                                        56, sf::Color(210, 210, 210),
                                        dx + 46.f, diceY + 40.f));
                }
            }

            if (phase == Phase::FROZEN) {
                // Total
                sf::Text tt = makeText(font, "= " + std::to_string(f1 + f2), 30,
                                       sf::Color(220, 220, 100), 0, diceY + diceSz + 12);
                tt.setPosition(centCX - tt.getLocalBounds().width / 2.f,
                               diceY + diceSz + 12);
                window.draw(tt);

                // Bouton Suivant / Résultats
                bool isLast = (curPlayer + 1 >= N);
                drawRect(window, centCX - 90.f, WIN_H - BTN_H + 3, 180.f, BTN_H - 6,
                         sf::Color(45, 145, 45), sf::Color::Black, 2.f);
                sf::Text bt = makeText(font, isLast ? "Resultats" : "Suivant",
                                       20, sf::Color::White, 0, WIN_H - BTN_H + 13);
                bt.setPosition(centCX - bt.getLocalBounds().width / 2.f,
                               WIN_H - BTN_H + 13);
                window.draw(bt);
            } else {
                sf::Text at = makeText(font, "Lancement en cours...", 15,
                                       sf::Color(110, 110, 135), 0, WIN_H - BTN_H + 14);
                at.setPosition(centCX - at.getLocalBounds().width / 2.f,
                               WIN_H - BTN_H + 14);
                window.draw(at);
            }

        } else {
            // ── Résultats finaux ───────────────────────────────────────────────
            sf::Text title = makeText(font, "Resultats finaux", 30,
                                      sf::Color(220, 180, 0), 0, 30);
            title.setPosition(centCX - title.getLocalBounds().width / 2.f, 30);
            window.draw(title);

            float sy = 105.f;
            for (int i = 0; i < N; ++i) {
                bool win = (i == winnerIdx);
                int  tot = d1s[i] + d2s[i];
                std::ostringstream oss;
                oss << noms[i] << "  :  " << d1s[i] << " + " << d2s[i]
                    << " = " << tot << (win ? "  ◀ GAGNANT" : "");
                sf::Color c = win ? sf::Color(80, 230, 80) : PLAYER_COLORS[i];
                sf::Text lt = makeText(font, oss.str(), win ? 22u : 17u, c, 0, sy);
                lt.setPosition(centCX - lt.getLocalBounds().width / 2.f, sy);
                window.draw(lt);
                sy += win ? 42.f : 34.f;
            }

            drawRect(window, centCX - 90.f, WIN_H - BTN_H + 3, 180.f, BTN_H - 6,
                     sf::Color(170, 130, 0), sf::Color::Black, 2.f);
            sf::Text bt = makeText(font, "Quitter", 20, sf::Color(16, 16, 16),
                                   0, WIN_H - BTN_H + 13);
            bt.setPosition(centCX - bt.getLocalBounds().width / 2.f, WIN_H - BTN_H + 13);
            window.draw(bt);
        }

        window.display();
    }

    if (winnerIdx >= 0) std::cout << noms[winnerIdx] << std::endl;
    return 0;
}
