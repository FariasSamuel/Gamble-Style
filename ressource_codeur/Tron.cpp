#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

/*
	1) Installer SFML : sudo apt install libsfml-dev
	2) Compiler : g++ Tron.cpp -o Tron -lsfml-graphics -lsfml-window -lsfml-system
	3) Lancer : ./Tron
	4) Jouer : Fleches pour le rouge, ZQSD pour le vert, ESC pour pause
*/

using namespace sf;

// ===================== GLOBAL =====================
int W, H;
int speed = 6;
std::vector<std::vector<uint8_t>> field;

// ===================== PLAYER =====================
struct Player {
    int x, y, dir;
    Color color;

    Player(Color c, int x0, int y0, int d)
        : color(c), x(x0), y(y0), dir(d) {}

    void tick() {
        if (dir == 0) y++;
        if (dir == 1) x--;
        if (dir == 2) x++;
        if (dir == 3) y--;

        if (x < 0) x = 0;
        if (x >= W) x = W - 1;
        if (y < 0) y = 0;
        if (y >= H) y = H - 1;
    }
};

// ===================== GAME STATE =====================
enum GameState { Playing, Paused, GameOver };
enum Result { None, RedLost, GreenLost, Tie };

// ===================== RESET =====================
void resetGame(Player &p1, Player &p2, RenderTexture &t) {
    field.assign(W, std::vector<uint8_t>(H, 0));

    p1 = Player(Color::Red, 3 * W / 4, H / 2, 1);
    p2 = Player(Color::Green, W / 4, H / 2, 2);

    t.clear(Color::Black);
    t.display();
}

// ===================== MAIN =====================
int main() {
    // Fullscreen adaptatif
    VideoMode desktop = VideoMode::getDesktopMode();
    W = desktop.width;
    H = desktop.height;

    RenderWindow window(desktop, "Tron Game", Style::Fullscreen);
    window.setFramerateLimit(60);

    RenderTexture t;
    t.create(W, H);
    Sprite sprite(t.getTexture());

    Player p1(Color::Red, W / 4, H / 2, 2);
    Player p2(Color::Green, 3 * W / 4, H / 2, 1);

    resetGame(p1, p2, t);

    GameState state = Playing;
    Result result = None;

    // ===== UI =====
    Font font;
    font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");

    Text message("", font, 50);
    message.setFillColor(Color::White);

    RectangleShape btn1(Vector2f(250, 70));
    RectangleShape btn2(Vector2f(250, 70));
    RectangleShape btn3(Vector2f(250, 70));

    btn1.setFillColor(Color(50,150,50));
    btn2.setFillColor(Color(150,50,50));
    btn3.setFillColor(Color(50,50,150));

    Text txt1("", font, 25);
    Text txt2("", font, 25);
    Text txt3("", font, 25);

    // ===================== LOOP =====================
    while (window.isOpen()) {
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed)
                window.close();

            // ESC = pause
            if (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape) {
                if (state == Playing) state = Paused;
                else if (state == Paused) state = Playing;
            }

            // CLIC BOUTONS
            if (e.type == Event::MouseButtonPressed) {
                Vector2f m(e.mouseButton.x, e.mouseButton.y);

                if (btn1.getGlobalBounds().contains(m)) {
                    if (state == GameOver || state == Paused) {
                        resetGame(p1, p2, t);
                        state = Playing;
                        result = None;
                    }
                }

                if (btn2.getGlobalBounds().contains(m)) {
                    window.close();
                }

                if (state == Paused && btn3.getGlobalBounds().contains(m)) {
                    state = Playing;
                }
            }
        }

        // ===== INPUT =====
        if (state == Playing) {
            if (Keyboard::isKeyPressed(Keyboard::Left) && p1.dir != 2) p1.dir = 1;
            if (Keyboard::isKeyPressed(Keyboard::Right) && p1.dir != 1) p1.dir = 2;
            if (Keyboard::isKeyPressed(Keyboard::Up) && p1.dir != 0) p1.dir = 3;
            if (Keyboard::isKeyPressed(Keyboard::Down) && p1.dir != 3) p1.dir = 0;

            if (Keyboard::isKeyPressed(Keyboard::Q) && p2.dir != 2) p2.dir = 1;
            if (Keyboard::isKeyPressed(Keyboard::D) && p2.dir != 1) p2.dir = 2;
            if (Keyboard::isKeyPressed(Keyboard::Z) && p2.dir != 0) p2.dir = 3;
            if (Keyboard::isKeyPressed(Keyboard::S) && p2.dir != 3) p2.dir = 0;
        }

        // ===== GAME =====
        if (state == Playing) {
            for (int i = 0; i < speed; i++) {
                p1.tick();
                p2.tick();

                bool dead1 = field[p1.x][p1.y];
                bool dead2 = field[p2.x][p2.y];

                if (dead1 && dead2) { result = Tie; state = GameOver; }
                else if (dead1) { result = RedLost; state = GameOver; }
                else if (dead2) { result = GreenLost; state = GameOver; }

                field[p1.x][p1.y] = 1;
                field[p2.x][p2.y] = 1;

                CircleShape c(3);
                c.setPosition(p1.x, p1.y);
                c.setFillColor(p1.color);
                t.draw(c);

                c.setPosition(p2.x, p2.y);
                c.setFillColor(p2.color);
                t.draw(c);
            }
            t.display();
        }

        // ===== DRAW =====
        window.clear();
        window.draw(sprite);

        // ===== OVERLAY =====
        if (state != Playing) {
            RectangleShape overlay(Vector2f(W, H));
            overlay.setFillColor(Color(0,0,0,180));
            window.draw(overlay);

            // message
            std::string msg;
            if (state == GameOver) {
                if (result == RedLost) msg = "Rouge a perdu";
                else if (result == GreenLost) msg = "Vert a perdu";
                else msg = "Egalite !";
            } else {
                msg = "Pause";
            }

            message.setString(msg);
            message.setPosition(W/2 - 150, H/4);
            window.draw(message);

            // boutons
            btn1.setPosition(W/2 - 125, H/2);
            btn2.setPosition(W/2 - 125, H/2 + 100);

            txt1.setString("Rejouer");
            txt2.setString("Quitter");

            txt1.setPosition(btn1.getPosition().x + 50, btn1.getPosition().y + 20);
            txt2.setPosition(btn2.getPosition().x + 50, btn2.getPosition().y + 20);

            window.draw(btn1);
            window.draw(btn2);
            window.draw(txt1);
            window.draw(txt2);

            // bouton continuer uniquement en pause
            if (state == Paused) {
                btn3.setPosition(W/2 - 125, H/2 + 200);
                txt3.setString("Continuer");
                txt3.setPosition(btn3.getPosition().x + 40, btn3.getPosition().y + 20);

                window.draw(btn3);
                window.draw(txt3);
            }
        }

        window.display();
    }

    return 0;
}