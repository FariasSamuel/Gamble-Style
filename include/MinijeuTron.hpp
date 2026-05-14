/**
 * @file MinijeuTron.hpp
 * @brief Classe MinijeuTron implémentant le jeu Tron via SFML.
 * @project GambleStyle — GM4 INSA Rouen Normandie
 */
#ifndef MINIJEU_TRON_HPP
#define MINIJEU_TRON_HPP

#include "Minijeu.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdint>

class Joueur;

/**
 * @class MinijeuTron
 * @brief Hérite de Minijeu. Ouvre une fenêtre SFML pour un duel 1v1.
 */
class MinijeuTron : public Minijeu {
public:
    MinijeuTron();
    ~MinijeuTron() override = default;

    const char* getNom() const override;

    /**
     * @brief Lance la fenêtre SFML, gère la partie, et retourne le vainqueur.
     * @param j1 Joueur 1 (contrôle Rouge - Flèches)
     * @param j2 Joueur 2 (contrôle Vert - ZQSD)
     */
    Joueur* jouer(Joueur* j1, Joueur* j2) override;

private:
    // ── Structures internes propres au jeu Tron ────────────────────────
    enum GameState { Playing, Paused, GameOver };
    enum Result { None, RedLost, GreenLost, Tie };

    struct TronMoto {
        int x, y, dir;
        sf::Color color;

        TronMoto(sf::Color c, int x0, int y0, int d);
        void tick(int W, int H);
    };

    void resetGame(TronMoto& p1, TronMoto& p2, std::vector<std::vector<uint8_t>>& field, int W, int H, Result& result, GameState& state);
};

#endif // MINIJEU_TRON_HPP
