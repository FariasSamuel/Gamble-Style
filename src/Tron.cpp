// Tron.cpp
// Mini-jeu Tron : deux joueurs pilotent un serpent lumineux dans une arène.
// Le premier joueur à percuter un mur ou la traînée adverse perd.
//
// Lancement depuis GambleStyle (via popen) :
//   ./bin/Tron "Joueur1" "Joueur2"
// Le nom du gagnant est imprimé sur stdout à la fermeture.
//
// Contrôles :
//   Joueur 1 (vert)   : Z / Q / S / D
//   Joueur 2 (rouge)  : Flèches directionnelles
//   Navigation menus  : Haut/Bas, Espace/Entrée

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

/*
    Compilation (depuis ressource_codeur/) :
        g++ Tron.cpp -o Tron -lsfml-graphics -lsfml-window -lsfml-system

    Depuis GambleStyle :
        ./build/Tron "Alice" "Bob"

    Navigation menus : Haut/Bas pour naviguer, Espace/Entrée pour valider.
    Contrôles en jeu : Flèches = Joueur 2 (rouge) | Z/Q/S/D = Joueur 1 (vert)
    Le nom du gagnant est écrit sur stdout à la fermeture.
*/

using namespace sf;

// ── Global ────────────────────────────────────────────────────────────────────
int W, H;
int speed = 5;
std::vector<std::vector<uint8_t>> field;

// ── Player ────────────────────────────────────────────────────────────────────
struct Player {
    int x, y, dir;
    Color color;
    Player(Color c, int x0, int y0, int d) : color(c), x(x0), y(y0), dir(d) {}
    void tick() {
        if (dir == 0) y++;
        if (dir == 1) x--;
        if (dir == 2) x++;
        if (dir == 3) y--;
        x = std::max(0, std::min(x, W-1));
        y = std::max(0, std::min(y, H-1));
    }
};

// ── States ────────────────────────────────────────────────────────────────────
enum GameState { Waiting, Playing, Paused, GameOver };
enum Result    { None, RedLost, GreenLost, Tie };

// ── Reset ─────────────────────────────────────────────────────────────────────
void resetGame(Player& p1, Player& p2, RenderTexture& t) {
    field.assign(W, std::vector<uint8_t>(H, 0));
    p1 = Player(Color::Red,   3*W/4, H/2, 1);
    p2 = Player(Color::Green, W/4,   H/2, 2);
    t.clear(Color::Black);
    t.display();
}

// ── Button highlight helper ───────────────────────────────────────────────────
static Color brighten(Color c, int delta = 60) {
    return Color(
        static_cast<uint8_t>(std::min(255, (int)c.r + delta)),
        static_cast<uint8_t>(std::min(255, (int)c.g + delta)),
        static_cast<uint8_t>(std::min(255, (int)c.b + delta))
    );
}

static void drawButton(RenderWindow& w, RectangleShape btn, const Text& txt, bool selected) {
    if (selected) {
        btn.setFillColor(brighten(btn.getFillColor()));
        btn.setOutlineThickness(4.f);
    }
    w.draw(btn);
    w.draw(txt);
}

// ─────────────────────────────────────────────────────────────────────────────
int main(int argc, char* argv[])
{
    std::string name1 = (argc >= 2) ? argv[1] : "Joueur 1";
    std::string name2 = (argc >= 3) ? argv[2] : "Joueur 2";

    VideoMode desktop = VideoMode::getDesktopMode();
    W = desktop.width;
    H = desktop.height;

    // Borderless fullscreen avoids focus issues when launched as child process
    RenderWindow window(desktop, "Tron \xe2\x80\x93 " + name1 + " vs " + name2,
                        Style::None);
    window.setPosition(Vector2i(0, 0));
    window.requestFocus();
    window.setFramerateLimit(60);

    RenderTexture t;
    t.create(W, H);
    Sprite sprite(t.getTexture());

    Player p1(Color::Red,   W/4,   H/2, 2);
    Player p2(Color::Green, 3*W/4, H/2, 1);
    resetGame(p1, p2, t);

    GameState state  = Waiting;
    Result    result = None;
    std::string winner;

    // selectedButton: index of focused button in current overlay
    // Waiting  : 0=Start, 1=Quit
    // Paused   : 0=Resume, 1=Quit
    // GameOver : 0=Quit
    int selectedButton = 0;

    // ── Font & UI ─────────────────────────────────────────────────────────────
    Font font;
    font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");

    Text msgTitle("TRON", font, 80);
    msgTitle.setFillColor(Color::Cyan);

    Text msgResult("", font, 52);
    msgResult.setFillColor(Color::White);

    // ── Button base colours ───────────────────────────────────────────────────
    const Color C_START(40, 160, 40);
    const Color C_QUIT(150, 50, 50);
    const Color C_RESUME(50, 50, 150);

    auto makeBtn = [](Color fill, Vector2f size) {
        RectangleShape b(size);
        b.setFillColor(fill);
        b.setOutlineColor(Color::White);
        b.setOutlineThickness(2.f);
        return b;
    };
    auto makeTxt = [&](const std::string& s, unsigned sz) {
        Text tx(s, font, sz);
        tx.setFillColor(Color::White);
        return tx;
    };

    RectangleShape btnStart  = makeBtn(C_START,  Vector2f(280, 70));
    RectangleShape btnQuit   = makeBtn(C_QUIT,   Vector2f(250, 70));
    RectangleShape btnResume = makeBtn(C_RESUME, Vector2f(250, 70));

    Text txtStart  = makeTxt("Lancer le jeu", 24);
    Text txtQuit   = makeTxt("Quitter",       24);
    Text txtResume = makeTxt("Continuer",     24);

    // Navigation hint (shown in all overlays)
    Text hint("[Haut/Bas] naviguer   [Espace/Entr\xc3\xa9" "e] valider", font, 18);
    hint.setFillColor(Color(160, 160, 160));

    // ── Loop ──────────────────────────────────────────────────────────────────
    while (window.isOpen())
    {
        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed) { window.close(); break; }

            if (e.type == Event::KeyPressed) {
                auto k = e.key.code;

                // Escape: pause / resume
                if (k == Keyboard::Escape) {
                    if      (state == Playing) { state = Paused;  selectedButton = 0; }
                    else if (state == Paused)  { state = Playing; }
                }

                // Navigate buttons (only when not playing)
                if (state != Playing) {
                    int numBtns = (state == Waiting) ? 2
                                : (state == Paused)  ? 2
                                                     : 1; // GameOver
                    if (k == Keyboard::Up)
                        selectedButton = (selectedButton - 1 + numBtns) % numBtns;
                    if (k == Keyboard::Down)
                        selectedButton = (selectedButton + 1) % numBtns;

                    // Activate
                    if (k == Keyboard::Return || k == Keyboard::Space) {
                        if (state == Waiting) {
                            if (selectedButton == 0) { state = Playing; }
                            else { if (!winner.empty()) std::cout << winner << std::endl;
                                   window.close(); }
                        } else if (state == Paused) {
                            if (selectedButton == 0) { state = Playing; }
                            else { if (!winner.empty()) std::cout << winner << std::endl;
                                   window.close(); }
                        } else { // GameOver
                            if (!winner.empty()) std::cout << winner << std::endl;
                            window.close();
                        }
                    }
                }
            }

            // Mouse (secondary)
            if (e.type == Event::MouseButtonPressed) {
                Vector2f m(float(e.mouseButton.x), float(e.mouseButton.y));
                if (state == Waiting  && btnStart.getGlobalBounds().contains(m)) state = Playing;
                if (btnQuit.getGlobalBounds().contains(m)) {
                    if (!winner.empty()) std::cout << winner << std::endl;
                    window.close();
                }
                if (state == Paused && btnResume.getGlobalBounds().contains(m)) state = Playing;
            }
        }

        // ── Player input (in game only) ───────────────────────────────────────
        // p1 = Red   = Arrow keys  (name2 in the legend — see header)
        // p2 = Green = Z/Q/S/D    (name1 in the legend)
        if (state == Playing) {
            if (Keyboard::isKeyPressed(Keyboard::Left)  && p1.dir != 2) p1.dir = 1;
            if (Keyboard::isKeyPressed(Keyboard::Right) && p1.dir != 1) p1.dir = 2;
            if (Keyboard::isKeyPressed(Keyboard::Up)    && p1.dir != 0) p1.dir = 3;
            if (Keyboard::isKeyPressed(Keyboard::Down)  && p1.dir != 3) p1.dir = 0;

            if (Keyboard::isKeyPressed(Keyboard::Q) && p2.dir != 2) p2.dir = 1;
            if (Keyboard::isKeyPressed(Keyboard::D) && p2.dir != 1) p2.dir = 2;
            if (Keyboard::isKeyPressed(Keyboard::Z) && p2.dir != 0) p2.dir = 3;
            if (Keyboard::isKeyPressed(Keyboard::S) && p2.dir != 3) p2.dir = 0;
        }

        // ── Physics ───────────────────────────────────────────────────────────
        if (state == Playing) {
            for (int i = 0; i < speed; ++i) {
                p1.tick(); p2.tick();
                bool d1 = field[p1.x][p1.y];
                bool d2 = field[p2.x][p2.y];
                if (d1 && d2) result = Tie;
                else if (d1)  result = RedLost;
                else if (d2)  result = GreenLost;
                if (result != None) { state = GameOver; selectedButton = 0; break; }
                field[p1.x][p1.y] = 1;
                field[p2.x][p2.y] = 1;
                CircleShape c(3);
                c.setPosition(float(p1.x), float(p1.y));
                c.setFillColor(p1.color); t.draw(c);
                c.setPosition(float(p2.x), float(p2.y));
                c.setFillColor(p2.color); t.draw(c);
            }
            t.display();
            // name1 = Green (p2), name2 = Red (p1)
            if (state == GameOver && winner.empty()) {
                if      (result == GreenLost) winner = name2;
                else if (result == RedLost)   winner = name1;
                else                           winner = name1; // tie → player 1
            }
        }

        // ── Draw ──────────────────────────────────────────────────────────────
        window.clear();
        window.draw(sprite);

        // ── Overlay when not playing ──────────────────────────────────────────
        if (state != Playing) {
            RectangleShape overlay{Vector2f(float(W), float(H))};
            overlay.setFillColor(Color(0, 0, 0, 200));
            window.draw(overlay);

            float cx = W / 2.f;
            float cy = H / 2.f;

            if (state == Waiting) {
                // Title
                FloatRect tb = msgTitle.getLocalBounds();
                msgTitle.setPosition(cx - tb.width/2, cy - 260);
                window.draw(msgTitle);

                // VS line
                Text vs(name1 + "  vs  " + name2, font, 34);
                vs.setFillColor(Color::White);
                FloatRect vb = vs.getLocalBounds();
                vs.setPosition(cx - vb.width/2, cy - 160);
                window.draw(vs);

                // Controls legend
                Text l1(name1 + "  =  Z / Q / S / D  (vert)", font, 22);
                l1.setFillColor(Color(100, 255, 100));
                FloatRect lb1 = l1.getLocalBounds();
                l1.setPosition(cx - lb1.width/2, cy - 90);
                window.draw(l1);

                Text l2(name2 + "  =  Fl\xc3\xa8" "ches directionnelles  (rouge)", font, 22);
                l2.setFillColor(Color(255, 100, 100));
                FloatRect lb2 = l2.getLocalBounds();
                l2.setPosition(cx - lb2.width/2, cy - 55);
                window.draw(l2);

                // Buttons
                btnStart.setPosition(cx - 140, cy + 20);
                FloatRect sb = txtStart.getLocalBounds();
                txtStart.setPosition(cx - sb.width/2, cy + 36);
                drawButton(window, btnStart, txtStart, selectedButton == 0);

                btnQuit.setPosition(cx - 125, cy + 120);
                FloatRect qb = txtQuit.getLocalBounds();
                txtQuit.setPosition(cx - qb.width/2, cy + 136);
                drawButton(window, btnQuit, txtQuit, selectedButton == 1);

            } else {
                // Paused or GameOver
                std::string msg;
                if (state == GameOver) {
                    if      (result == GreenLost) msg = name2 + " gagne !";
                    else if (result == RedLost)   msg = name1 + " gagne !";
                    else                           msg = "\xc3\x89galit\xc3\xa9 !";
                } else {
                    msg = "Pause";
                }
                msgResult.setString(msg);
                FloatRect mb = msgResult.getLocalBounds();
                msgResult.setPosition(cx - mb.width/2, cy - 180);
                window.draw(msgResult);

                if (state == Paused) {
                    // Resume (index 0)
                    btnResume.setPosition(cx - 125, cy - 30);
                    FloatRect reb = txtResume.getLocalBounds();
                    txtResume.setPosition(cx - reb.width/2, cy - 14);
                    drawButton(window, btnResume, txtResume, selectedButton == 0);

                    // Quit (index 1)
                    btnQuit.setPosition(cx - 125, cy + 80);
                    FloatRect qb = txtQuit.getLocalBounds();
                    txtQuit.setPosition(cx - qb.width/2, cy + 96);
                    drawButton(window, btnQuit, txtQuit, selectedButton == 1);
                } else {
                    // GameOver: only Quit (index 0)
                    btnQuit.setPosition(cx - 125, cy + 50);
                    FloatRect qb = txtQuit.getLocalBounds();
                    txtQuit.setPosition(cx - qb.width/2, cy + 66);
                    drawButton(window, btnQuit, txtQuit, selectedButton == 0);
                }
            }

            // Navigation hint
            FloatRect hb = hint.getLocalBounds();
            hint.setPosition(cx - hb.width/2, float(H) - 60.f);
            window.draw(hint);
        }

        window.display();
    }

    if (!winner.empty())
        std::cout << winner << std::endl;

    return 0;
}
