#include "PlateauSFML.hpp"

#include "CaseDepart.hpp"
#include "CaseAleatoire.hpp"
#include "CasePropriete.hpp"
#include "CasePrison.hpp"
#include "CaseGotoPrison.hpp"
#include "CaseEvenement.hpp"
#include "Joueur.hpp"

#include <sstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

// ── Couleurs joueurs (fallback si texture absente) ────────────────────────────
static const sf::Color PLAYER_COLORS[4] = {
    sf::Color(220,  60,  60),
    sf::Color( 60, 120, 220),
    sf::Color( 60, 200,  60),
    sf::Color(220, 180,   0)
};

// ─────────────────────────────────────────────────────────────────────────────
//  Constructeur / Destructeur
// ─────────────────────────────────────────────────────────────────────────────
PlateauSFML::PlateauSFML() : Plateau(1500.f)
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

PlateauSFML::~PlateauSFML()
{
    cleanupGame();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Utilitaires statiques
// ─────────────────────────────────────────────────────────────────────────────
sf::Text PlateauSFML::makeText(const sf::Font& f, const std::string& s,
                               unsigned sz, sf::Color col, float x, float y)
{
    sf::Text t;
    t.setFont(f); t.setString(s); t.setCharacterSize(sz);
    t.setFillColor(col); t.setPosition(x, y);
    return t;
}

void PlateauSFML::drawRect(sf::RenderWindow& w,
                           float x, float y, float bw, float bh,
                           sf::Color fill, sf::Color outline, float thick)
{
    sf::RectangleShape r(sf::Vector2f(bw, bh));
    r.setPosition(x, y);
    r.setFillColor(fill);
    r.setOutlineColor(outline);
    r.setOutlineThickness(thick);
    w.draw(r);
}

std::string PlateauSFML::condStr(Condition c)
{
    if (c == Condition::FAILLITE)    return "FAILLITE";
    if (c == Condition::BANQUEROUTE) return "BANQ.";
    return "RICHE";
}
sf::Color PlateauSFML::condColor(Condition c)
{
    if (c == Condition::FAILLITE)    return sf::Color(200, 60, 60);
    if (c == Condition::BANQUEROUTE) return sf::Color(220,150,  0);
    return sf::Color(60, 200, 60);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Helpers de jeu
// ─────────────────────────────────────────────────────────────────────────────
void PlateauSFML::addMsg(const std::string& s)
{
    msgLog_.push_front(s);
    if (msgLog_.size() > 4) msgLog_.pop_back();
}

int PlateauSFML::countActifs() const
{
    int n = 0;
    for (auto j : joueurs)
        if (j->conditionfinanciere() != Condition::FAILLITE) ++n;
    return n;
}

void PlateauSFML::nextActivePlayer()
{
    if (joueurs.empty()) return;
    int tries = (int)joueurs.size();
    do {
        currentPlayer_ = (currentPlayer_ + 1) % (int)joueurs.size();
    } while (--tries > 0 &&
             joueurs[currentPlayer_]->conditionfinanciere() == Condition::FAILLITE);
}

sf::Vector2f PlateauSFML::caseCenter(int idx) const
{
    auto [nx, ny] = cfg_.caseNormPos(idx);
    return { nx * BOARD_SIZE, ny * BOARD_SIZE };
}

// ─────────────────────────────────────────────────────────────────────────────
//  Chargement des textures
// ─────────────────────────────────────────────────────────────────────────────
void PlateauSFML::loadTextures()
{
    // Fond du plateau
    if (cfg_.board_image != "none" && !cfg_.board_image.empty())
        hasBoardTex_ = texBoard_.loadFromFile(cfg_.board_image);
    if (hasBoardTex_) {
        spriteBoard_.setTexture(texBoard_);
        spriteBoard_.setScale(float(BOARD_SIZE) / texBoard_.getSize().x,
                              float(BOARD_SIZE) / texBoard_.getSize().y);
    }

    // Textures des cases
    int nb = (int)cfg_.cases.size();
    cellTex_.resize(nb);
    hasCellTex_.resize(nb, false);
    for (int i = 0; i < nb; ++i) {
        const std::string& p = cfg_.cases[i].texture;
        if (p != "none" && !p.empty())
            hasCellTex_[i] = cellTex_[i].loadFromFile(p);
    }

    // Sprites joueurs
    for (int i = 0; i < 4; ++i)
        texJoueur_[i].loadFromFile("assets/joueur" + std::to_string(i + 1) + ".png");

    // Faces de dés
    for (int i = 0; i < 6; ++i)
        texDe_[i].loadFromFile("assets/de" + std::to_string(i + 1) + ".png");
}

// ─────────────────────────────────────────────────────────────────────────────
//  Nettoyage entre deux parties
// ─────────────────────────────────────────────────────────────────────────────
void PlateauSFML::cleanupGame()
{
    for (auto c : cases)   delete c;
    for (auto j : joueurs) delete j;
    cases.clear();
    joueurs.clear();

    for (int i = 0; i < 4; ++i) posJoueurs_[i] = 0;
    currentPlayer_    = 0;
    hasDiceResult_    = false;
    msgLog_.clear();
    gagnantFinal_     = nullptr;
    pendingBuy_       = nullptr;
    pendingGamble_    = nullptr;
    miseSelectionnee_ = 100;
    state_            = State::PLAYING;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Construction des objets de jeu depuis cfg_
// ─────────────────────────────────────────────────────────────────────────────
void PlateauSFML::buildGame()
{
    cleanupGame();

    // Durée de partie
    int dur = 30;
    try { dur = std::stoi(dureeStr_.empty() ? "30" : dureeStr_); } catch (...) {}
    setTempsJeu(dur);

    // Joueurs
    for (int i = 0; i < nbJoueurs_; ++i)
        joueurs.push_back(new Joueur(noms_[i], 1500));

    // Cases
    CasePrison*     prisonCase = nullptr;
    CaseGotoPrison* gotoCase   = nullptr;

    for (int i = 0; i < (int)cfg_.cases.size(); ++i) {
        const CaseConfig& cc = cfg_.cases[i];
        Case* c = nullptr;
        switch (cc.type) {
            case CaseType::DEPART:
                c = new CaseDepart(cc.param > 0 ? cc.param : 200, i);
                break;
            case CaseType::EVENEMENT: {
                auto* ce = new CaseEvenement(i);
                if (!cc.command.empty()) ce->setGameName(cc.command);
                for (auto j : joueurs) ce->ajouterJoueur(j);
                c = ce;
                break;
            }
            case CaseType::PROPRIETE: {
                auto* cp = new CasePropriete(i, cc.name, cc.param > 0 ? cc.param : 100);
                cp->setCommand(cc.command);
                cp->setPlateau(this);
                c = cp;
                break;
            }
            case CaseType::PRISON:
                prisonCase = new CasePrison(i);
                c = prisonCase;
                break;
            case CaseType::GOTO_PRISON:
                gotoCase = new CaseGotoPrison(i);
                c = gotoCase;
                break;
            case CaseType::ALEATOIRE:
                c = new CaseAleatoire(i);
                break;
        }
        cases.push_back(c);
    }

    // Câblage prison ↔ goto_prison
    if (gotoCase && prisonCase) gotoCase->setPrisonCase(prisonCase);

    // Positions initiales (case 0 = DÉPART)
    for (int i = 0; i < nbJoueurs_; ++i) {
        posJoueurs_[i] = 0;
        joueurs[i]->setCaseActuelle(cases[0]);
    }

    // Ordre de jeu initial
    ordre();

    addMsg("Partie démarrée ! Tour de " + joueurs[0]->getNom());
}

// ─────────────────────────────────────────────────────────────────────────────
//  Écran de configuration (boucle bloquante)
// ─────────────────────────────────────────────────────────────────────────────
void PlateauSFML::runSetupScreen()
{
    activeField_ = -1;
    while (window_.isOpen()) {
        sf::Event ev;
        while (window_.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) { window_.close(); return; }

            // Saisie texte
            if (ev.type == sf::Event::TextEntered) {
                auto ch = ev.text.unicode;
                std::string* tgt = nullptr;
                if (activeField_ == 0)                         tgt = &dureeStr_;
                else if (activeField_ >= 1 && activeField_ <= 4) tgt = &noms_[activeField_-1];
                if (tgt) {
                    if (ch == 8 && !tgt->empty()) tgt->pop_back();
                    else if (ch >= 32 && ch < 127 && tgt->size() < 20)
                        *tgt += static_cast<char>(ch);
                }
            }

            if (ev.type != sf::Event::MouseButtonPressed ||
                ev.mouseButton.button != sf::Mouse::Left) continue;

            sf::Vector2f mouse(float(ev.mouseButton.x), float(ev.mouseButton.y));

            // Boutons nb joueurs
            for (int n = 2; n <= 4; ++n) {
                float bx = 300.f + (n-2)*120.f;
                if (sf::FloatRect(bx, 240.f, 90.f, 42.f).contains(mouse))
                { nbJoueurs_ = n; activeField_ = -1; }
            }
            // Champs noms
            for (int i = 0; i < nbJoueurs_; ++i) {
                float fy = 340.f + i*60.f;
                if (sf::FloatRect(300.f, fy, 380.f, 40.f).contains(mouse))
                    activeField_ = i + 1;
            }
            // Champ durée
            {
                float dy = 340.f + nbJoueurs_*60.f + 20.f;
                if (sf::FloatRect(300.f, dy, 180.f, 40.f).contains(mouse))
                    activeField_ = 0;
            }
            // Bouton Démarrer → sortie de la boucle setup
            {
                float sy = 340.f + nbJoueurs_*60.f + 80.f;
                if (sf::FloatRect(380.f, sy, 200.f, 50.f).contains(mouse)) return;
            }
        }

        window_.clear(sf::Color(22, 22, 32));
        renderSetupScreen();
        window_.display();
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  initialization()  — point d'entrée unique avant gamelooping()
// ─────────────────────────────────────────────────────────────────────────────
void PlateauSFML::initialization()
{
    // Fenêtre
    window_.create(sf::VideoMode(WIN_W, WIN_H), "Gamble Style", sf::Style::Close);
    window_.setFramerateLimit(60);

    // Police
    hasFont_ = font_.loadFromFile(
        "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf");

    // Configuration plateau
    cfg_     = BoardConfig::loadFromFile("board.txt");
    cellPx_  = float(BOARD_SIZE) / cfg_.grid_size;

    // Textures (nécessite cfg_ chargé)
    loadTextures();

    // Écran de configuration joueurs
    runSetupScreen();
    if (!window_.isOpen()) return;

    // Création des objets de jeu
    buildGame();
}

// ─────────────────────────────────────────────────────────────────────────────
//  doTurn()
// ─────────────────────────────────────────────────────────────────────────────
void PlateauSFML::doTurn()
{
    Joueur* j = joueurs[currentPlayer_];

    int de1  = (std::rand() % 6) + 1;
    int de2  = (std::rand() % 6) + 1;
    int roll = de1 + de2;
    lastDe1_ = de1; lastDe2_ = de2;
    hasDiceResult_ = true;

    int oldPos = posJoueurs_[currentPlayer_];
    int newPos = (oldPos + roll) % (int)cases.size();
    posJoueurs_[currentPlayer_] = newPos;
    j->setCaseActuelle(cases[newPos]);

    // Passage par DÉPART
    if (newPos < oldPos || (roll > 0 && newPos == 0)) {
        j->setCapital(j->getCapital() + 200);
        addMsg(j->getNom() + " passe par DÉPART  +200€");
    }

    {
        std::ostringstream oss;
        oss << j->getNom() << " : " << de1 << "+" << de2 << "=" << roll
            << "  →  " << cfg_.cases[newPos].name;
        addMsg(oss.str());
    }

    cases[newPos]->setJoueurActif(j);

    auto* cp  = dynamic_cast<CasePropriete*>(cases[newPos]);
    auto* cgp = dynamic_cast<CaseGotoPrison*>(cases[newPos]);

    if (cp) {
        cp->action();
        if (cp->achatPropose()) {
            pendingBuy_ = cp;
            addMsg("Acheter \"" + cp->getName() + "\" pour "
                   + std::to_string(cp->getPrix()) + "€ ?");
            state_ = State::BUY_PROMPT;
            return;
        }
        if (cp->gambleEnAttente()) {
            pendingGamble_    = cp;
            miseSelectionnee_ = 100;
            Joueur* prop = cp->getProprietaire();
            addMsg((prop ? prop->getNom() : "?")
                   + " : choisissez votre mise pour "
                   + "\"" + cp->getName() + "\"");
            state_ = State::GAMBLE_PROMPT;
            return;
        }
    } else if (cgp) {
        cgp->action();
        int prisonIdx = cfg_.findCaseType(CaseType::PRISON);
        if (prisonIdx >= 0) posJoueurs_[currentPlayer_] = prisonIdx;
        addMsg(j->getNom() + " est envoyé en Prison !");
    } else {
        cases[newPos]->action();
        if (dynamic_cast<CasePrison*>(cases[newPos]))
            addMsg(j->getNom() + " est en Prison (visite).");
    }

    if (j->conditionfinanciere() == Condition::FAILLITE)
        addMsg(j->getNom() + " est en FAILLITE !");

    if (countActifs() <= 1) {
        gagnantFinal_ = fin();
        state_ = State::GAMEOVER;
        addMsg("FIN ! Gagnant : "
               + (gagnantFinal_ ? gagnantFinal_->getNom() : "?"));
        return;
    }

    nextActivePlayer();
    addMsg("→ Tour de " + joueurs[currentPlayer_]->getNom());
}

// ─────────────────────────────────────────────────────────────────────────────
//  gamelooping()  — boucle principale SFML
// ─────────────────────────────────────────────────────────────────────────────
void PlateauSFML::gamelooping()
{
    while (window_.isOpen())
    {
        sf::Event ev;
        while (window_.pollEvent(ev))
        {
            if (ev.type == sf::Event::Closed) { window_.close(); break; }

            if (ev.type != sf::Event::MouseButtonPressed ||
                ev.mouseButton.button != sf::Mouse::Left) continue;

            sf::Vector2f mouse(float(ev.mouseButton.x), float(ev.mouseButton.y));

            // ── PLAYING : bouton "Lancer le dé" ──────────────────────────────
            if (state_ == State::PLAYING) {
                float btnX = float(BOARD_SIZE) + 60.f;
                float btnY = float(WIN_H) - 80.f;
                if (sf::FloatRect(btnX, btnY, 300.f, 58.f).contains(mouse))
                    doTurn();
            }

            // ── BUY_PROMPT : Acheter / Passer ─────────────────────────────────
            else if (state_ == State::BUY_PROMPT && pendingBuy_) {
                sf::FloatRect rOui(420.f, 460.f, 130.f, 48.f);
                sf::FloatRect rNon(600.f, 460.f, 130.f, 48.f);

                auto finishBuy = [&]() {
                    pendingBuy_ = nullptr;
                    if (countActifs() <= 1) {
                        gagnantFinal_ = fin();
                        state_ = State::GAMEOVER;
                    } else {
                        nextActivePlayer();
                        addMsg("→ Tour de " + joueurs[currentPlayer_]->getNom());
                        state_ = State::PLAYING;
                    }
                };

                if (rOui.contains(mouse)) {
                    Joueur* j = joueurs[currentPlayer_];
                    if (j->getCapital() >= pendingBuy_->getPrix()) {
                        j->acheter(pendingBuy_);
                        addMsg(j->getNom() + " achète \"" + pendingBuy_->getName()
                               + "\"  -" + std::to_string(pendingBuy_->getPrix()) + "€");
                    } else {
                        addMsg(j->getNom() + " n'a pas assez !");
                    }
                    finishBuy();
                } else if (rNon.contains(mouse)) {
                    addMsg(joueurs[currentPlayer_]->getNom() + " refuse.");
                    finishBuy();
                }
            }

            // ── GAMBLE_PROMPT : sélection de la mise ─────────────────────────
            else if (state_ == State::GAMBLE_PROMPT && pendingGamble_) {
                static const int MISES[4] = {50, 100, 150, 200};
                float cx = WIN_W / 2.f;

                // Boutons de mise (4 boutons, centrés)
                for (int k = 0; k < 4; ++k) {
                    float bx = cx - 235.f + k * 120.f;
                    if (sf::FloatRect(bx, 430.f, 105.f, 46.f).contains(mouse))
                        miseSelectionnee_ = MISES[k];
                }

                // Bouton Confirmer
                if (sf::FloatRect(cx - 90.f, 495.f, 180.f, 50.f).contains(mouse)) {
                    pendingGamble_->confirmerMise(miseSelectionnee_);

                    Joueur* visiteur = joueurs[currentPlayer_];
                    Joueur* prop     = pendingGamble_->getProprietaire();
                    if (pendingGamble_->minijeuLance()) {
                        // Déduire le message de résultat depuis les capitaux
                        addMsg("Jeu terminé — mise : "
                               + std::to_string(miseSelectionnee_) + "€");
                    }
                    pendingGamble_ = nullptr;

                    if (visiteur->conditionfinanciere() == Condition::FAILLITE)
                        addMsg(visiteur->getNom() + " est en FAILLITE !");
                    if (prop && prop->conditionfinanciere() == Condition::FAILLITE)
                        addMsg(prop->getNom() + " est en FAILLITE !");

                    if (countActifs() <= 1) {
                        gagnantFinal_ = fin();
                        state_ = State::GAMEOVER;
                        addMsg("FIN ! Gagnant : "
                               + (gagnantFinal_ ? gagnantFinal_->getNom() : "?"));
                    } else {
                        nextActivePlayer();
                        addMsg("→ Tour de " + joueurs[currentPlayer_]->getNom());
                        state_ = State::PLAYING;
                    }
                }
            }

            // ── GAMEOVER : Nouvelle partie ────────────────────────────────────
            else if (state_ == State::GAMEOVER) {
                float cx = WIN_W / 2.f;
                if (sf::FloatRect(cx - 140.f, 520.f, 280.f, 54.f).contains(mouse)) {
                    runSetupScreen();
                    if (window_.isOpen()) buildGame();
                }
            }
        }

        if (!window_.isOpen()) break;

        // ── Rendu ─────────────────────────────────────────────────────────────
        window_.clear(sf::Color(18, 18, 28));
        renderBoard();
        renderLog();
        renderPanel();

        if (state_ == State::BUY_PROMPT)    renderBuyPrompt();
        if (state_ == State::GAMBLE_PROMPT) renderGamblePrompt();
        if (state_ == State::GAMEOVER)      renderGameOver();

        window_.display();
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  renderBoard()  – plateau + cases assombries + auras + pions
// ─────────────────────────────────────────────────────────────────────────────
void PlateauSFML::renderBoard()
{
    // Fond plateau
    if (hasBoardTex_)
        window_.draw(spriteBoard_);
    else
        drawRect(window_, 0, 0, float(BOARD_SIZE), float(BOARD_SIZE),
                 sf::Color(38, 38, 52));

    // Textures de chaque case + voile d'obscurité
    for (int i = 0; i < (int)cfg_.cases.size(); ++i) {
        if (!hasCellTex_[i]) continue;
        auto [nx, ny] = cfg_.caseNormPos(i);
        float cx = nx * BOARD_SIZE - cellPx_ * 0.5f;
        float cy = ny * BOARD_SIZE - cellPx_ * 0.5f;

        sf::Sprite sp(cellTex_[i]);
        sp.setScale(cellPx_ / cellTex_[i].getSize().x,
                    cellPx_ / cellTex_[i].getSize().y);
        sp.setPosition(cx, cy);
        window_.draw(sp);

        // Voile d'obscurité (dark_coeff lu dans board.txt)
        if (cfg_.dark_coeff > 0.f) {
            sf::RectangleShape veil(sf::Vector2f(cellPx_, cellPx_));
            veil.setPosition(cx, cy);
            veil.setFillColor(sf::Color(0, 0, 0,
                              static_cast<uint8_t>(cfg_.dark_coeff * 255)));
            window_.draw(veil);
        }
    }

    // Surbrillance colorée des cases appartenant à un joueur
    for (int i = 0; i < (int)cases.size(); ++i) {
        auto* cp = dynamic_cast<CasePropriete*>(cases[i]);
        if (!cp || !cp->getProprietaire()) continue;

        int colorIdx = -1;
        for (int k = 0; k < (int)joueurs.size(); ++k)
            if (joueurs[k] == cp->getProprietaire()) { colorIdx = k; break; }
        if (colorIdx < 0) continue;

        auto [nx, ny] = cfg_.caseNormPos(i);
        float cx = nx * BOARD_SIZE - cellPx_ * 0.5f;
        float cy = ny * BOARD_SIZE - cellPx_ * 0.5f;

        sf::Color pc = PLAYER_COLORS[colorIdx];
        // Léger fond teinté + bordure épaisse de la couleur du propriétaire
        drawRect(window_, cx, cy, cellPx_, cellPx_,
                 sf::Color(pc.r, pc.g, pc.b, 35),
                 sf::Color(pc.r, pc.g, pc.b, 210), 3.f);
    }

    // Surbrillance de la case active (après le voile, avant les pions)
    if (!joueurs.empty()) {
        sf::Vector2f c = caseCenter(posJoueurs_[currentPlayer_]);
        sf::RectangleShape hl(sf::Vector2f(cellPx_ - 4, cellPx_ - 4));
        hl.setFillColor(sf::Color(255, 255, 0, 45));
        hl.setOutlineColor(sf::Color(255, 220, 0, 200));
        hl.setOutlineThickness(3.f);
        hl.setPosition(c.x - cellPx_*0.5f + 2, c.y - cellPx_*0.5f + 2);
        window_.draw(hl);
    }

    // Pions joueurs — dessinés au-dessus du voile (luminosité totale)
    static const float OFX[4] = {-0.21f,  0.21f, -0.21f,  0.21f};
    static const float OFY[4] = {-0.21f, -0.21f,  0.21f,  0.21f};
    float pionSz = cellPx_ * 0.40f; // légèrement plus grands qu'avant

    for (int i = 0; i < (int)joueurs.size(); ++i) {
        if (joueurs[i]->conditionfinanciere() == Condition::FAILLITE) continue;

        sf::Vector2f cp = caseCenter(posJoueurs_[i]);
        float px = cp.x + OFX[i] * cellPx_;
        float py = cp.y + OFY[i] * cellPx_;
        bool  isActive = (i == currentPlayer_);

        // Aura colorée (permet de repérer le joueur même sur case sombre)
        float glowR = pionSz * 0.72f;
        sf::CircleShape glow(glowR);
        glow.setFillColor(sf::Color(PLAYER_COLORS[i].r,
                                    PLAYER_COLORS[i].g,
                                    PLAYER_COLORS[i].b,
                                    isActive ? 160u : 90u));
        glow.setPosition(px - glowR, py - glowR);
        window_.draw(glow);

        // Sprite ou cercle de secours
        if (texJoueur_[i].getSize().x > 0) {
            float ratio = float(texJoueur_[i].getSize().x) / texJoueur_[i].getSize().y;
            float h = pionSz;
            float w = h * ratio;
            sf::Sprite sp(texJoueur_[i]);
            sp.setScale(w / texJoueur_[i].getSize().x,
                        h / texJoueur_[i].getSize().y);
            sp.setPosition(px - w * 0.5f, py - h * 0.5f);
            window_.draw(sp);
        } else {
            float r = pionSz * 0.45f;
            sf::CircleShape circ(r);
            circ.setFillColor(PLAYER_COLORS[i]);
            circ.setOutlineColor(sf::Color::White);
            circ.setOutlineThickness(isActive ? 3.f : 2.f);
            circ.setPosition(px - r, py - r);
            window_.draw(circ);
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  renderLog()  – bandeau de messages sous le plateau
// ─────────────────────────────────────────────────────────────────────────────
void PlateauSFML::renderLog()
{
    float y0 = float(BOARD_SIZE);
    drawRect(window_, 0, y0, float(BOARD_SIZE), float(LOG_H),
             sf::Color(10, 10, 20, 230));
    drawRect(window_, 0, y0, float(BOARD_SIZE), 2, sf::Color(65, 65, 90));

    if (!hasFont_) return;
    for (int i = 0; i < (int)msgLog_.size(); ++i) {
        float fy  = y0 + 10.f + i * 28.f;
        sf::Color col = (i == 0) ? sf::Color(240, 240, 100)
                                 : sf::Color(160, 160, 160);
        window_.draw(makeText(font_, msgLog_[i], 15, col, 10.f, fy));
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  renderPanel()  – panneau droit : dés + cartes joueurs + bouton
// ─────────────────────────────────────────────────────────────────────────────
void PlateauSFML::renderPanel()
{
    float px0 = float(BOARD_SIZE);
    drawRect(window_, px0, 0, float(PANEL_W), float(WIN_H),
             sf::Color(14, 14, 24));
    drawRect(window_, px0, 0, 2, float(WIN_H), sf::Color(65, 65, 90));

    if (!hasFont_) return;

    // Tour en cours
    if (!joueurs.empty()) {
        std::string turnStr = "Tour de " + joueurs[currentPlayer_]->getNom();
        window_.draw(makeText(font_, turnStr, 17,
                              PLAYER_COLORS[currentPlayer_], px0 + 18.f, 12.f));
    }

    // ── Dés ──────────────────────────────────────────────────────────────────
    float diceSz = 92.f;
    float diceY  = 45.f;
    float dx1 = px0 + 18.f;
    float dx2 = px0 + 130.f;

    if (hasDiceResult_) {
        for (int d = 0; d < 2; ++d) {
            int face = (d == 0 ? lastDe1_ : lastDe2_) - 1;
            float dxN = (d == 0) ? dx1 : dx2;
            sf::Sprite sp(texDe_[face]);
            sp.setScale(diceSz / texDe_[face].getSize().x,
                        diceSz / texDe_[face].getSize().y);
            sp.setPosition(dxN, diceY);
            window_.draw(sp);
        }
        window_.draw(makeText(font_, "+", 26, sf::Color(190, 190, 190),
                              dx1 + diceSz + 4, diceY + 33));
        window_.draw(makeText(font_, "= " + std::to_string(lastDe1_ + lastDe2_),
                              20, sf::Color(220, 220, 100),
                              px0 + 252.f, diceY + 38));
    } else {
        for (float dxN : {dx1, dx2}) {
            drawRect(window_, dxN, diceY, diceSz, diceSz,
                     sf::Color(36, 36, 52), sf::Color(70, 70, 95), 2.f);
            window_.draw(makeText(font_, "?", 44, sf::Color(90, 90, 110),
                                  dxN + 30, diceY + 19));
        }
    }

    drawRect(window_, px0 + 10, 150, float(PANEL_W) - 20, 1,
             sf::Color(55, 55, 75));

    // ── Cartes joueurs ────────────────────────────────────────────────────────
    float cardY = 160.f;
    float cardH = std::min(110.f,
                           float(WIN_H - 90 - 160) / std::max(1, (int)joueurs.size()));
    float cardW = float(PANEL_W) - 16.f;

    for (int i = 0; i < (int)joueurs.size(); ++i) {
        Joueur*   j   = joueurs[i];
        Condition c   = j->conditionfinanciere();
        bool      cur = (i == currentPlayer_);

        drawRect(window_, px0 + 8, cardY, cardW, cardH - 3.f,
                 cur ? sf::Color(40, 40, 60) : sf::Color(22, 22, 36),
                 cur ? PLAYER_COLORS[i] : sf::Color(50, 50, 70),
                 cur ? 2.f : 1.f);
        // Bandeau couleur
        drawRect(window_, px0 + 8, cardY, 5.f, cardH - 3.f, PLAYER_COLORS[i]);

        // Miniature sprite
        if (texJoueur_[i].getSize().x > 0) {
            sf::Sprite th(texJoueur_[i]);
            th.setScale(24.f / texJoueur_[i].getSize().x,
                        27.f / texJoueur_[i].getSize().y);
            th.setPosition(px0 + 18.f, cardY + 5.f);
            window_.draw(th);
        }

        float tx = px0 + 50.f;
        sf::Color nc = (c == Condition::FAILLITE)
                       ? sf::Color(100, 100, 100) : sf::Color(225, 225, 225);
        window_.draw(makeText(font_, j->getNom(),   15, nc, tx, cardY + 4.f));
        window_.draw(makeText(font_, std::to_string(j->getCapital()) + " €",
                              14, sf::Color(80, 210, 80), tx, cardY + 24.f));
        window_.draw(makeText(font_, condStr(c), 12, condColor(c), tx, cardY + 44.f));
        window_.draw(makeText(font_, std::to_string(j->getNbProprietes()) + " prop.",
                              12, sf::Color(130, 165, 210), tx + 95, cardY + 44.f));
        cardY += cardH;
    }

    // ── Bouton "Lancer le dé" ─────────────────────────────────────────────────
    if (state_ == State::PLAYING) {
        float btnX = px0 + 60.f;
        float btnY = float(WIN_H) - 80.f;
        drawRect(window_, btnX, btnY, 300.f, 58.f,
                 sf::Color(170, 130, 0), sf::Color::Black, 2.f);
        window_.draw(makeText(font_, "Lancer le dé", 22, sf::Color(16, 16, 16),
                              btnX + 32.f, btnY + 14.f));
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  renderSetupScreen()  – écran de configuration (appelé par runSetupScreen)
// ─────────────────────────────────────────────────────────────────────────────
void PlateauSFML::renderSetupScreen()
{
    if (!hasFont_) return;

    // Titre
    sf::Text title = makeText(font_, "GAMBLE  STYLE", 54,
                              sf::Color(220, 180, 0), 0, 60);
    title.setStyle(sf::Text::Bold);
    sf::FloatRect tb = title.getLocalBounds();
    title.setPosition((WIN_W - tb.width) / 2.f, 60.f);
    window_.draw(title);

    sf::Text sub = makeText(font_, "Nouvelle partie", 22,
                            sf::Color(170, 170, 170), 0, 138);
    sf::FloatRect sb = sub.getLocalBounds();
    sub.setPosition((WIN_W - sb.width) / 2.f, 138.f);
    window_.draw(sub);

    drawRect(window_, 180, 184, 730, 2, sf::Color(75, 75, 95));

    // Nombre de joueurs
    window_.draw(makeText(font_, "Nombre de joueurs :", 20,
                          sf::Color(215, 215, 215), 160, 250));
    for (int n = 2; n <= 4; ++n) {
        float bx = 300.f + (n-2) * 120.f;
        bool sel = (n == nbJoueurs_);
        drawRect(window_, bx, 240.f, 90.f, 42.f,
                 sel ? sf::Color(220, 180, 0) : sf::Color(48, 48, 65),
                 sf::Color(120, 120, 145), 1.5f);
        window_.draw(makeText(font_, std::to_string(n) + " joueurs", 15,
                              sel ? sf::Color(18, 18, 18) : sf::Color(205, 205, 205),
                              bx + 8.f, 250.f));
    }

    // Noms
    window_.draw(makeText(font_, "Noms :", 20,
                          sf::Color(215, 215, 215), 160, 316));
    for (int i = 0; i < nbJoueurs_; ++i) {
        float fy  = 340.f + i * 60.f;
        bool  act = (activeField_ == i + 1);
        sf::CircleShape dot(8.f);
        dot.setFillColor(PLAYER_COLORS[i]);
        dot.setPosition(268.f, fy + 12.f);
        window_.draw(dot);
        drawRect(window_, 300.f, fy, 380.f, 40.f,
                 sf::Color(35, 35, 50),
                 act ? sf::Color(220, 180, 0) : sf::Color(85, 85, 105),
                 act ? 2.f : 1.f);
        window_.draw(makeText(font_, noms_[i] + (act ? "|" : ""), 18,
                              sf::Color(235, 235, 235), 310.f, fy + 8.f));
    }

    // Durée
    float dy    = 340.f + nbJoueurs_ * 60.f + 20.f;
    bool actDur = (activeField_ == 0);
    window_.draw(makeText(font_, "Durée (min) :", 20,
                          sf::Color(215, 215, 215), 160, dy + 6.f));
    drawRect(window_, 300.f, dy, 180.f, 40.f,
             sf::Color(35, 35, 50),
             actDur ? sf::Color(220, 180, 0) : sf::Color(85, 85, 105),
             actDur ? 2.f : 1.f);
    window_.draw(makeText(font_, dureeStr_ + (actDur ? "|" : ""), 18,
                          sf::Color(235, 235, 235), 310.f, dy + 8.f));

    // Bouton Démarrer
    float sy = 340.f + nbJoueurs_ * 60.f + 80.f;
    drawRect(window_, 380.f, sy, 200.f, 50.f,
             sf::Color(220, 180, 0), sf::Color::Black, 2.f);
    window_.draw(makeText(font_, "Démarrer", 22, sf::Color(18, 18, 18),
                          430.f, sy + 10.f));
}

// ─────────────────────────────────────────────────────────────────────────────
//  renderBuyPrompt()
// ─────────────────────────────────────────────────────────────────────────────
void PlateauSFML::renderBuyPrompt()
{
    if (!pendingBuy_ || !hasFont_) return;

    sf::RectangleShape dim{sf::Vector2f(float(WIN_W), float(WIN_H))};
    dim.setFillColor(sf::Color(0, 0, 0, 160));
    window_.draw(dim);

    float cx = WIN_W / 2.f;
    drawRect(window_, cx - 280, 305, 560, 265,
             sf::Color(26, 26, 40), sf::Color(220, 180, 0), 2.f);

    window_.draw(makeText(font_, "Achat de propriété", 23,
                          sf::Color(220, 180, 0), cx - 210, 325));
    window_.draw(makeText(font_, "\"" + pendingBuy_->getName() + "\"",
                          20, sf::Color(235, 235, 235), cx - 210, 368));
    window_.draw(makeText(font_, "Prix : " + std::to_string(pendingBuy_->getPrix()) + " €",
                          17, sf::Color(75, 205, 75), cx - 210, 400));
    window_.draw(makeText(font_,
                          "Votre capital : "
                          + std::to_string(joueurs[currentPlayer_]->getCapital()) + " €",
                          15, sf::Color(165, 165, 165), cx - 210, 426));

    drawRect(window_, 420, 460, 130, 48,
             sf::Color(45, 145, 45), sf::Color::Black, 2.f);
    window_.draw(makeText(font_, "Acheter", 20, sf::Color::White, 442, 470));

    drawRect(window_, 600, 460, 130, 48,
             sf::Color(145, 45, 45), sf::Color::Black, 2.f);
    window_.draw(makeText(font_, "Passer", 20, sf::Color::White, 624, 470));
}

// ─────────────────────────────────────────────────────────────────────────────
//  renderGamblePrompt()  – overlay choix de la mise par le propriétaire
// ─────────────────────────────────────────────────────────────────────────────
void PlateauSFML::renderGamblePrompt()
{
    if (!pendingGamble_ || !hasFont_) return;

    sf::RectangleShape dim{sf::Vector2f(float(WIN_W), float(WIN_H))};
    dim.setFillColor(sf::Color(0, 0, 0, 165));
    window_.draw(dim);

    float cx = WIN_W / 2.f;
    drawRect(window_, cx - 290, 250, 580, 315,
             sf::Color(24, 24, 38), sf::Color(220, 180, 0), 2.f);

    // Titre
    window_.draw(makeText(font_, "Choisissez la mise", 23,
                          sf::Color(220, 180, 0), cx - 220, 268));

    // Propriété et joueurs impliqués
    Joueur* prop    = pendingGamble_->getProprietaire();
    Joueur* visiteur = joueurs[currentPlayer_];
    window_.draw(makeText(font_,
                          "\"" + pendingGamble_->getName() + "\"  —  propriété de "
                          + (prop ? prop->getNom() : "?"),
                          17, sf::Color(230, 230, 230), cx - 220, 310));
    window_.draw(makeText(font_,
                          visiteur->getNom() + " affronte "
                          + (prop ? prop->getNom() : "?"),
                          15, sf::Color(165, 165, 165), cx - 220, 340));

    // Règles de la mise
    window_.draw(makeText(font_, "Mise (50 – 200 €) :", 16,
                          sf::Color(190, 190, 190), cx - 220, 374));

    // Boutons de mise
    static const int MISES[4] = {50, 100, 150, 200};
    for (int k = 0; k < 4; ++k) {
        float bx = cx - 235.f + k * 120.f;
        bool  sel = (MISES[k] == miseSelectionnee_);
        drawRect(window_, bx, 430.f, 105.f, 46.f,
                 sel ? sf::Color(180, 140, 0) : sf::Color(40, 40, 58),
                 sel ? sf::Color(220, 180, 0) : sf::Color(80, 80, 105),
                 sel ? 2.f : 1.f);
        window_.draw(makeText(font_, std::to_string(MISES[k]) + " €",
                              18,
                              sel ? sf::Color(20, 20, 20) : sf::Color(210, 210, 210),
                              bx + 18.f, 440.f));
    }

    // Bouton Confirmer
    drawRect(window_, cx - 90.f, 495.f, 180.f, 50.f,
             sf::Color(45, 145, 45), sf::Color::Black, 2.f);
    window_.draw(makeText(font_, "Confirmer", 20, sf::Color::White, cx - 62.f, 507.f));
}

// ─────────────────────────────────────────────────────────────────────────────
//  renderGameOver()
// ─────────────────────────────────────────────────────────────────────────────
void PlateauSFML::renderGameOver()
{
    if (!hasFont_) return;

    sf::RectangleShape dim{sf::Vector2f(float(WIN_W), float(WIN_H))};
    dim.setFillColor(sf::Color(0, 0, 0, 185));
    window_.draw(dim);

    float cx = WIN_W / 2.f;
    drawRect(window_, cx - 280, 175, 560, 390,
             sf::Color(20, 20, 36), sf::Color(220, 180, 0), 3.f);

    sf::Text t1 = makeText(font_, "FIN DE PARTIE", 42,
                           sf::Color(220, 180, 0), 0, 205);
    t1.setPosition((WIN_W - t1.getLocalBounds().width) / 2.f, 205);
    window_.draw(t1);

    std::string wn = gagnantFinal_ ? gagnantFinal_->getNom() : "?";
    sf::Text t2 = makeText(font_, "Gagnant : " + wn, 26,
                           sf::Color(75, 225, 75), 0, 295);
    t2.setPosition((WIN_W - t2.getLocalBounds().width) / 2.f, 295);
    window_.draw(t2);

    float sy = 348.f;
    for (int i = 0; i < (int)joueurs.size(); ++i) {
        int score = joueurs[i]->getCapital() + joueurs[i]->getValPropriete();
        std::ostringstream oss;
        oss << joueurs[i]->getNom() << "  :  " << score << " €";
        sf::Text st = makeText(font_, oss.str(), 17, PLAYER_COLORS[i], 0, sy);
        st.setPosition((WIN_W - st.getLocalBounds().width) / 2.f, sy);
        window_.draw(st);
        sy += 32.f;
    }

    drawRect(window_, cx - 140, 520, 280, 54,
             sf::Color(220, 180, 0), sf::Color::Black, 2.f);
    sf::Text bt = makeText(font_, "Nouvelle partie", 21,
                           sf::Color(18, 18, 18), 0, 531);
    bt.setPosition((WIN_W - bt.getLocalBounds().width) / 2.f, 531);
    window_.draw(bt);
}
