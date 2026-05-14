/**
 * @file MinijeuTron.cpp
 * @brief Implémentation du mini-jeu Tron en SFML.
 * @project GambleStyle — GM4 INSA Rouen Normandie
 */
#include "MinijeuTron.hpp"
#include "Joueur.hpp"
#include <iostream>

using namespace sf;

// ═══════════════════════════════════════════════════════════════
// Méthodes de la structure interne TronMoto
// ═══════════════════════════════════════════════════════════════

MinijeuTron::TronMoto::TronMoto(Color c, int x0, int y0, int d)
    : color(c), x(x0), y(y0), dir(d) {}

void MinijeuTron::TronMoto::tick(int W, int H) {
    if (dir == 0) y++;        // Bas
    if (dir == 1) x--;        // Gauche
    if (dir == 2) x++;        // Droite
    if (dir == 3) y--;        // Haut

    // Gestion des murs (téléportation de l'autre côté)
    if (x < 0) x = W - 1;
    if (x >= W) x = 0;
    if (y < 0) y = H - 1;
    if (y >= H) y = 0;
}

// ═══════════════════════════════════════════════════════════════
// Implémentation de MinijeuTron
// ═══════════════════════════════════════════════════════════════

MinijeuTron::MinijeuTron() {}

const char* MinijeuTron::getNom() const { 
    return "Tron"; 
}

void MinijeuTron::resetGame(TronMoto& p1, TronMoto& p2, std::vector<std::vector<uint8_t>>& field, int W, int H, Result& result, GameState& state) {
    for (int i = 0; i < W; i++) {
        for (int j = 0; j < H; j++) {
            field[i][j] = 0;
        }
    }
    p1.x = W / 3;     p1.y = H / 2; p1.dir = 2; // Rouge démarre à gauche, va à droite
    p2.x = 2 * W / 3; p2.y = H / 2; p2.dir = 1; // Vert démarre à droite, va à gauche
    result = None;
    state = Playing;
}

Joueur* MinijeuTron::jouer(Joueur* j1, Joueur* j2) {
    // 1. Paramètres locaux (remplacent les variables globales)
    const int W = 600;
    const int H = 480;
    const int speed = 5; // Vitesse ajustable
    std::vector<std::vector<uint8_t>> field(W, std::vector<uint8_t>(H, 0));

    // 2. Création de la fenêtre propre au mini-jeu
    RenderWindow window(VideoMode(W, H), "GambleStyle - Duel Tron !");
    window.setFramerateLimit(60);

    TronMoto p1(Color::Red, W / 3, H / 2, 2);   // Joueur 1 (Rouge)
    TronMoto p2(Color::Green, 2 * W / 3, H / 2, 1); // Joueur 2 (Vert)
    
    GameState state;
    Result result;
    resetGame(p1, p2, field, W, H, result, state);

    // Chargement de la police (Assurez-vous d'avoir ce fichier dans assets/)
    Font font;
    if (!font.loadFromFile("assets/arial.ttf")) {
        std::cerr << "Erreur : Impossible de charger la police assets/arial.ttf" << std::endl;
    }
    Text message("", font, 30);
    message.setFillColor(Color::White);

    bool p1_tick = false;
    bool p2_tick = false;

    // 3. Boucle SFML du mini-jeu
    while (window.isOpen()) {
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) {
                window.close();
                return j1; // Par défaut si fermeture brutale
            }

            if (e.type == Event::KeyPressed) {
                if (e.key.code == Keyboard::Escape) {
                    state = (state == Playing) ? Paused : Playing;
                }
            }
        }

        if (state == Playing) {
            // Contrôles J1 (Rouge - Flèches)
            if (Keyboard::isKeyPressed(Keyboard::Left)  && p1.dir != 2) p1.dir = 1;
            if (Keyboard::isKeyPressed(Keyboard::Right) && p1.dir != 1) p1.dir = 2;
            if (Keyboard::isKeyPressed(Keyboard::Up)    && p1.dir != 0) p1.dir = 3;
            if (Keyboard::isKeyPressed(Keyboard::Down)  && p1.dir != 3) p1.dir = 0;

            // Contrôles J2 (Vert - ZQSD)
            if (Keyboard::isKeyPressed(Keyboard::Q) && p2.dir != 2) p2.dir = 1;
            if (Keyboard::isKeyPressed(Keyboard::D) && p2.dir != 1) p2.dir = 2;
            if (Keyboard::isKeyPressed(Keyboard::Z) && p2.dir != 0) p2.dir = 3;
            if (Keyboard::isKeyPressed(Keyboard::S) && p2.dir != 3) p2.dir = 0;

            // Déplacement
            for (int i = 0; i < speed; i++) {
                p1.tick(W, H);
                p2.tick(W, H);

                // Collisions
                if (field[p1.x][p1.y] == 1) p1_tick = true;
                if (field[p2.x][p2.y] == 1) p2_tick = true;

                if (p1_tick && p2_tick) { result = Tie;       state = GameOver; break; }
                if (p1_tick)            { result = RedLost;   state = GameOver; break; }
                if (p2_tick)            { result = GreenLost; state = GameOver; break; }

                field[p1.x][p1.y] = 1;
                field[p2.x][p2.y] = 1;
            }
        }

        // Affichage
        window.clear();

        // Dessin des traces
        VertexArray lines(Points);
        for (int i = 0; i < W; i++) {
            for (int j = 0; j < H; j++) {
                if (field[i][j] == 1) {
                    lines.append(Vertex(Vector2f(i, j), Color::White));
                }
            }
        }
        window.draw(lines);

        // Si le jeu est terminé, on affiche le menu de fin
        if (state == GameOver) {
            RectangleShape overlay(Vector2f(W, H));
            overlay.setFillColor(Color(0, 0, 0, 180));
            window.draw(overlay);

            std::string msg;
            if (result == RedLost)        msg = j2->getNom() + " (Vert) a gagne !";
            else if (result == GreenLost) msg = j1->getNom() + " (Rouge) a gagne !";
            else                          msg = "Egalite !";

            message.setString(msg + "\n\nAppuyez sur ESPACE pour Quitter\nAppuyez sur R pour Rejouer");
            message.setPosition(W / 4, H / 3);
            window.draw(message);

            // Choix du joueur à la fin du duel
            if (Keyboard::isKeyPressed(Keyboard::Space)) {
                window.close();
                if (result == RedLost) return j2;
                if (result == GreenLost) return j1;
                return j1; // En cas d'égalité, j1 gagne par défaut (ou relancez)
            }
            if (Keyboard::isKeyPressed(Keyboard::R)) {
                p1_tick = false;
                p2_tick = false;
                resetGame(p1, p2, field, W, H, result, state);
            }
        }

        window.display();
    }
    return j1;
}
