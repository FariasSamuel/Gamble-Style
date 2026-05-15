// PlateauSFML.cpp
// Couche de présentation SFML du jeu GambleStyle.
// Hérite de Plateau (logique métier) et ajoute : fenêtre SFML, textures,
// machine à états (PLAYING / MOVING / BUY_PROMPT / GAMBLE_PROMPT / …),
// et toute la logique de rendu.
//
// Flux principal :
//   initialization() → runSetupScreen() → buildGame()
//   gamelooping()    → boucle SFML → doTurn() → resolveAction() → advanceTurn()

#include "PlateauSFML.hpp"

#include "CaseDepart.hpp"
#include "CaseAleatoire.hpp"
#include "CasePropriete.hpp"
#include "CasePrison.hpp"
#include "CaseGotoPrison.hpp"
#include "CaseEvenement.hpp"
#include "Joueur.hpp"
#include "CarteAnniversaire.hpp"
#include "CarteArgent.hpp"
#include "CarteDeplacement.hpp"
#include "CarteDoubleGain.hpp"
#include "CarteSortiePrison.hpp"

#include <numeric>  // std::iota

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
    t.setFont(f);
    t.setString(sf::String::fromUtf8(s.begin(), s.end()));
    t.setCharacterSize(sz);
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

    for (int i = 0; i < 4; ++i) { posJoueurs_[i] = 0; doubleStreak_[i] = 0; }
    currentPlayer_       = 0;
    hasDiceResult_       = false;
    msgLog_.clear();
    gagnantFinal_        = nullptr;
    pendingBuy_          = nullptr;
    pendingGamble_       = nullptr;
    miseSelectionnee_    = 100;
    autoRoll_            = false;
    pendingSortiePrison_ = nullptr;
    pendingDoubleGainCard_ = nullptr;
    pendingDoubleGain_   = false;
    state_               = State::PLAYING;
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
    prisonIdx_ = cfg_.findCaseType(CaseType::PRISON);

    for (int i = 0; i < (int)cfg_.cases.size(); ++i) {
        const CaseConfig& cc = cfg_.cases[i];
        Case* c = nullptr;
        switch (cc.type) {
            case CaseType::DEPART:
                c = new CaseDepart(cc.param > 0 ? cc.param : 200, i);
                break;
            case CaseType::EVENEMENT: {
                auto* ce = new CaseEvenement(i);
                ce->setMise(cc.param > 0 ? cc.param : 100);
                ce->setCommand(cc.command);
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
            case CaseType::ALEATOIRE: {
                auto* ca = new CaseAleatoire(i);
                int amt = (cc.param > 0) ? cc.param : 50;
                ca->ajouterCarte(new CarteAnniversaire(nullptr, this, amt));
                ca->ajouterCarte(new CarteArgent(nullptr, amt * 2));
                ca->ajouterCarte(new CarteDoubleGain("Double Gain", ""));
                ca->ajouterCarte(new CarteDeplacement(this, 2, 6));
                ca->ajouterCarte(new CarteSortiePrison(nullptr));
                c = ca;
                break;
            }
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

            sf::Vector2f mouse = window_.mapPixelToCoords(
                sf::Vector2i(ev.mouseButton.x, ev.mouseButton.y), gameView_);

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
    // Fenêtre plein écran
    auto modes = sf::VideoMode::getFullscreenModes();
    sf::VideoMode desktop = modes.empty() ? sf::VideoMode(1920, 1080) : modes[0];
    window_.create(desktop, "Gamble Style", sf::Style::Fullscreen);
    window_.setFramerateLimit(60);

    // Vue : coordonnées de conception 1090x800 → résolution physique (letterbox)
    {
        float sx = float(desktop.width)  / float(WIN_W);
        float sy = float(desktop.height) / float(WIN_H);
        float sc = std::min(sx, sy);
        float vpW = float(WIN_W) * sc / desktop.width;
        float vpH = float(WIN_H) * sc / desktop.height;
        gameView_ = sf::View(sf::FloatRect(0.f, 0.f, float(WIN_W), float(WIN_H)));
        gameView_.setViewport(sf::FloatRect(
            (1.f - vpW) / 2.f, (1.f - vpH) / 2.f, vpW, vpH));
        window_.setView(gameView_);
    }

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
//  advanceTurn()  — appelé après résolution de toute action de case
// ─────────────────────────────────────────────────────────────────────────────
void PlateauSFML::advanceTurn()
{
    if (countActifs() <= 1) {
        gagnantFinal_ = fin();
        state_ = State::GAMEOVER;
        addMsg("FIN ! Gagnant : "
               + (gagnantFinal_ ? gagnantFinal_->getNom() : "?"));
        return;
    }
    // Double en cours → le joueur rejoue (sauf triple double déjà géré)
    if (doubleStreak_[currentPlayer_] > 0) {
        addMsg("Double ! " + joueurs[currentPlayer_]->getNom() + " rejoue !");
        autoRoll_ = true;
        autoRollClock_.restart();
        state_ = State::PLAYING;
        return;
    }
    nextActivePlayer();
    addMsg("→ Tour de " + joueurs[currentPlayer_]->getNom());
    state_ = State::PLAYING;
}

// ─────────────────────────────────────────────────────────────────────────────
//  doTurn()
// ─────────────────────────────────────────────────────────────────────────────
void PlateauSFML::doTurn()
{
    Joueur* j = joueurs[currentPlayer_];

    // ── Prison : libération automatique au 3e tour ────────────────────────────
    int cpt = j->getCptPrison();
    if (cpt == 3) {
        j->setCptPrison(0);
        cpt = 0;
        addMsg(j->getNom() + " est libéré automatiquement de prison !");
    }

    // ── Lancer des dés ────────────────────────────────────────────────────────
    int de1  = (std::rand() % 6) + 1;
    int de2  = (std::rand() % 6) + 1;
    int roll = de1 + de2;
    bool isDouble = (de1 == de2);
    lastDe1_ = de1; lastDe2_ = de2;
    hasDiceResult_ = true;

    // ── Gestion prison en cours ───────────────────────────────────────────────
    if (cpt >= 1) {
        if (isDouble) {
            j->setCptPrison(0);
            addMsg(j->getNom() + " sort de prison avec un double !");
            // Mouvement ci-dessous, pas de streak de double
        } else {
            j->setCptPrison(cpt + 1);
            std::ostringstream oss;
            oss << j->getNom() << " reste en prison ("
                << j->getCptPrison() << "/3) — "
                << de1 << "+" << de2;
            addMsg(oss.str());
            // Pas de mouvement
            doubleStreak_[currentPlayer_] = 0;
            nextActivePlayer();
            addMsg("→ Tour de " + joueurs[currentPlayer_]->getNom());
            return;
        }
    } else {
        // ── Détection triple double ───────────────────────────────────────────
        if (isDouble) {
            if (doubleStreak_[currentPlayer_] == 2) {
                // Triple double → prison immédiate
                doubleStreak_[currentPlayer_] = 0;
                j->setCptPrison(1);
                if (prisonIdx_ >= 0) {
                    posJoueurs_[currentPlayer_] = prisonIdx_;
                    j->setCaseActuelle(cases[prisonIdx_]);
                }
                addMsg(j->getNom() + " triple double → Prison directe !");
                nextActivePlayer();
                addMsg("→ Tour de " + joueurs[currentPlayer_]->getNom());
                return;
            }
            doubleStreak_[currentPlayer_]++;
        } else {
            doubleStreak_[currentPlayer_] = 0;
        }
    }

    // ── Mouvement ─────────────────────────────────────────────────────────────
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
            << (isDouble ? " ⬛" : "") << "  →  " << cfg_.cases[newPos].name;
        addMsg(oss.str());
    }

    cases[newPos]->setJoueurActif(j);

    // Pause 1 s : le joueur voit son pion en place avant la résolution
    state_ = State::MOVING;
    actionClock_.restart();
}

// ─────────────────────────────────────────────────────────────────────────────
//  resolveAction()  — déclenché après le délai d'affichage du mouvement
// ─────────────────────────────────────────────────────────────────────────────
void PlateauSFML::resolveAction()
{
    int     newPos = posJoueurs_[currentPlayer_];
    Joueur* j      = joueurs[currentPlayer_];

    auto* cp  = dynamic_cast<CasePropriete*>(cases[newPos]);
    auto* cgp = dynamic_cast<CaseGotoPrison*>(cases[newPos]);
    auto* ce  = dynamic_cast<CaseEvenement*>(cases[newPos]);
    auto* ca  = dynamic_cast<CaseAleatoire*>(cases[newPos]);

    if (cp) {
        cp->action();
        if (cp->achatPropose()) {
            pendingBuy_ = cp;
            addMsg("Acheter \"" + cp->getName() + "\" pour "
                   + std::to_string(cp->getPrix()) + "\xe2\x82\xac ?");
            state_ = State::BUY_PROMPT;
            return;
        }
        if (cp->gambleEnAttente()) {
            pendingGamble_    = cp;
            miseSelectionnee_ = 100;
            Joueur* prop = cp->getProprietaire();
            addMsg((prop ? prop->getNom() : "?")
                   + " : choisissez votre mise pour \""
                   + cp->getName() + "\"");
            state_ = State::GAMBLE_PROMPT;
            return;
        }
    } else if (cgp) {
        cgp->action();
        if (prisonIdx_ >= 0) {
            posJoueurs_[currentPlayer_] = prisonIdx_;
            j->setCaseActuelle(cases[prisonIdx_]);
        }
        j->setCptPrison(1);
        doubleStreak_[currentPlayer_] = 0;
        addMsg(j->getNom() + " est envoy\xc3\xa9 en Prison !");
    } else if (ce) {
        ce->action();
        if (ce->getGagnant())
            addMsg(ce->getGagnant()->getNom() + " remporte la bataille de d\xc3\xa9s !");
        else
            addMsg("\xc3\x89galit\xc3\xa9 \xc3\xa0 la bataille de d\xc3\xa9s !");
    } else if (ca) {
        int capBefore = j->getCapital();
        int posBefore = posJoueurs_[currentPlayer_];

        // Peek at card BEFORE drawing (front of queue)
        Carte* drawnCard = ca->getPremiereCarteDeFile();

        ca->action(); // draws card, calls setTitulaire, calls card->action()

        int capDelta  = j->getCapital() - capBefore;
        int posAfter  = posBefore;

        // Handle position change (CarteDeplacement)
        Case* newCase = j->getCaseActuelle();
        if (newCase) {
            int newCaseIdx = newCase->get_num_case();
            if (newCaseIdx != posBefore) {
                if (newCaseIdx < posBefore) {
                    j->setCapital(j->getCapital() + 200);
                    capDelta += 200;
                    addMsg(j->getNom() + " passe par D\xc3\x89PART +200\xe2\x82\xac (carte)");
                }
                posJoueurs_[currentPlayer_] = newCaseIdx;
                posAfter = newCaseIdx;
            }
        }

        // Prepare card overlay data then pause for player to read
        prepareCarteDisplay(drawnCard, j, capDelta, posBefore, posAfter);

        if (j->conditionfinanciere() == Condition::FAILLITE)
            addMsg(j->getNom() + " est en FAILLITE !");

        state_ = State::CARTE_DRAWN_PROMPT;
        return; // advanceTurn() called from the "Continuer" button
    } else {
        cases[newPos]->action();
        if (dynamic_cast<CasePrison*>(cases[newPos]))
            addMsg(j->getNom() + " est en Station Prison (visite).");
    }

    if (j->conditionfinanciere() == Condition::FAILLITE)
        addMsg(j->getNom() + " est en FAILLITE !");

    advanceTurn();
}

// ─────────────────────────────────────────────────────────────────────────────
//  gamelooping()  — boucle principale SFML
// ─────────────────────────────────────────────────────────────────────────────
void PlateauSFML::gamelooping()
{
    while (window_.isOpen())
    {
        // Délai 1 s entre déplacement et résolution de l'action de la case
        if (state_ == State::MOVING &&
            actionClock_.getElapsedTime().asSeconds() >= 1.f) {
            resolveAction();
        }

        // Auto-relance après double (1,5 s de pause pour voir le résultat)
        if (autoRoll_ && state_ == State::PLAYING &&
            autoRollClock_.getElapsedTime().asSeconds() > 1.5f) {
            autoRoll_ = false;
            doTurn();
        }

        sf::Event ev;
        while (window_.pollEvent(ev))
        {
            if (ev.type == sf::Event::Closed) { window_.close(); break; }

            if (ev.type == sf::Event::KeyPressed) {
                if (ev.key.code == sf::Keyboard::Escape) { window_.close(); break; }

                // Espace / Entrée ferme l'overlay carte
                if (state_ == State::CARTE_DRAWN_PROMPT &&
                    (ev.key.code == sf::Keyboard::Space ||
                     ev.key.code == sf::Keyboard::Return)) {
                    state_ = State::PLAYING;
                    advanceTurn();
                }
            }

            if (ev.type != sf::Event::MouseButtonPressed ||
                ev.mouseButton.button != sf::Mouse::Left) continue;

            sf::Vector2f mouse = window_.mapPixelToCoords(
                sf::Vector2i(ev.mouseButton.x, ev.mouseButton.y), gameView_);

            // ── PLAYING : bouton "Lancer le dé" ──────────────────────────────
            if (state_ == State::PLAYING) {
                float btnX = float(BOARD_SIZE) + 60.f;
                float btnY = float(WIN_H) - 80.f;
                if (sf::FloatRect(btnX, btnY, 300.f, 58.f).contains(mouse)) {
                    autoRoll_ = false;
                    // Prison + Sortie Prison card → ask first
                    Joueur* jj = joueurs[currentPlayer_];
                    if (jj->getCptPrison() > 0) {
                        CarteSortiePrison* sp = jj->getSortiePrison();
                        if (sp) {
                            pendingSortiePrison_ = sp;
                            state_ = State::USE_SORTIE_PRISON_PROMPT;
                        } else {
                            doTurn();
                        }
                    } else {
                        doTurn();
                    }
                }
            }

            // ── BUY_PROMPT : Acheter / Passer ─────────────────────────────────
            else if (state_ == State::BUY_PROMPT && pendingBuy_) {
                sf::FloatRect rOui(420.f, 460.f, 130.f, 48.f);
                sf::FloatRect rNon(600.f, 460.f, 130.f, 48.f);

                auto finishBuy = [&]() {
                    pendingBuy_ = nullptr;
                    if (joueurs[currentPlayer_]->conditionfinanciere() == Condition::FAILLITE)
                        addMsg(joueurs[currentPlayer_]->getNom() + " est en FAILLITE !");
                    advanceTurn();
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

                for (int k = 0; k < 4; ++k) {
                    float bx = cx - 235.f + k * 120.f;
                    if (sf::FloatRect(bx, 430.f, 105.f, 46.f).contains(mouse))
                        miseSelectionnee_ = MISES[k];
                }

                if (sf::FloatRect(cx - 90.f, 495.f, 180.f, 50.f).contains(mouse)) {
                    Joueur* visiteur = joueurs[currentPlayer_];
                    // Check for Double Gain card before launching the game
                    CarteDoubleGain* dg = visiteur->getDoubleGain();
                    if (dg && !pendingDoubleGain_) {
                        pendingDoubleGainCard_ = dg;
                        state_ = State::USE_DOUBLE_GAIN_PROMPT;
                    } else {
                        Joueur* prop = pendingGamble_->getProprietaire();
                        pendingGamble_->confirmerMise(miseSelectionnee_);
                        {
                            Joueur* g = pendingGamble_->getGagnantDernierJeu();
                            if (g)
                                addMsg(g->getNom() + " gagne ! (mise "
                                       + std::to_string(miseSelectionnee_)
                                       + "\xe2\x82\xac)");
                            else
                                addMsg("\xc3\x89galit\xc3\xa9 ! (mise "
                                       + std::to_string(miseSelectionnee_)
                                       + "\xe2\x82\xac)");
                        }

                        // Double Gain bonus: extra mise if visiteur won
                        if (pendingDoubleGain_) {
                            Joueur* g = pendingGamble_->getGagnantDernierJeu();
                            if (g == visiteur) {
                                visiteur->setCapital(visiteur->getCapital() + miseSelectionnee_);
                                addMsg(visiteur->getNom() + " Double Gain : +"
                                       + std::to_string(miseSelectionnee_) + "\xe2\x82\xac !");
                            }
                            pendingDoubleGain_    = false;
                            pendingDoubleGainCard_ = nullptr;
                        }

                        pendingGamble_ = nullptr;
                        if (visiteur->conditionfinanciere() == Condition::FAILLITE)
                            addMsg(visiteur->getNom() + " est en FAILLITE !");
                        if (prop && prop->conditionfinanciere() == Condition::FAILLITE)
                            addMsg(prop->getNom() + " est en FAILLITE !");
                        advanceTurn();
                    }
                }
            }

            // ── CARTE_DRAWN_PROMPT : bouton Continuer ─────────────────────────
            else if (state_ == State::CARTE_DRAWN_PROMPT) {
                float cx = WIN_W / 2.f, cy = WIN_H / 2.f;
                float ph = 370.f, py = cy - ph / 2.f;
                float btnX = cx - 90.f, btnY = py + ph - 70.f;
                if (sf::FloatRect(btnX, btnY, 180.f, 50.f).contains(mouse)) {
                    state_ = State::PLAYING;
                    advanceTurn();
                }
            }

            // ── USE_SORTIE_PRISON_PROMPT ──────────────────────────────────────
            else if (state_ == State::USE_SORTIE_PRISON_PROMPT) {
                float cx = WIN_W / 2.f;
                // "Utiliser" button
                if (sf::FloatRect(cx - 270.f, 430.f, 220.f, 50.f).contains(mouse)) {
                    Joueur* jj = joueurs[currentPlayer_];
                    jj->retirerCarte(pendingSortiePrison_);
                    jj->setCptPrison(0);
                    pendingSortiePrison_ = nullptr;
                    state_ = State::PLAYING;
                    doTurn();
                }
                // "Jouer normalement" button
                else if (sf::FloatRect(cx + 50.f, 430.f, 220.f, 50.f).contains(mouse)) {
                    pendingSortiePrison_ = nullptr;
                    state_ = State::PLAYING;
                    doTurn();
                }
            }

            // ── USE_DOUBLE_GAIN_PROMPT ────────────────────────────────────────
            else if (state_ == State::USE_DOUBLE_GAIN_PROMPT) {
                float cx = WIN_W / 2.f;
                // "Utiliser" button
                if (sf::FloatRect(cx - 270.f, 430.f, 220.f, 50.f).contains(mouse)) {
                    Joueur* visiteur = joueurs[currentPlayer_];
                    visiteur->retirerCarte(pendingDoubleGainCard_);
                    pendingDoubleGain_ = true;
                    state_ = State::GAMBLE_PROMPT;
                    // Re-show GAMBLE_PROMPT; next "Confirmer" click will run the game
                }
                // "Ne pas utiliser" button
                else if (sf::FloatRect(cx + 50.f, 430.f, 220.f, 50.f).contains(mouse)) {
                    pendingDoubleGain_    = false;
                    pendingDoubleGainCard_ = nullptr;
                    state_ = State::GAMBLE_PROMPT;
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
        window_.setView(gameView_);
        renderBoard();
        renderLog();
        renderPanel();

        if (state_ == State::BUY_PROMPT)               renderBuyPrompt();
        if (state_ == State::GAMBLE_PROMPT)            renderGamblePrompt();
        if (state_ == State::USE_SORTIE_PRISON_PROMPT) renderUseSortiePrisonPrompt();
        if (state_ == State::USE_DOUBLE_GAIN_PROMPT)   renderUseDoubleGainPrompt();
        if (state_ == State::CARTE_DRAWN_PROMPT)       renderCarteDrawnPrompt();
        if (state_ == State::GAMEOVER)                 renderGameOver();

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
        window_.draw(makeText(font_, std::to_string(j->getCapital()) + " \xe2\x82\xac",
                              14, sf::Color(80, 210, 80), tx, cardY + 24.f));
        window_.draw(makeText(font_, condStr(c), 12, condColor(c), tx, cardY + 44.f));
        window_.draw(makeText(font_, std::to_string(j->getNbProprietes()) + " prop.",
                              12, sf::Color(130, 165, 210), tx + 95, cardY + 44.f));

        // Card inventory line
        std::string cartesStr;
        if (j->getSortiePrison()) cartesStr += "Sortie Prison ";
        if (j->getDoubleGain())   cartesStr += "Double Gain";
        if (!cartesStr.empty())
            window_.draw(makeText(font_, cartesStr, 11,
                                  sf::Color(220, 200, 80), tx, cardY + 64.f));

        cardY += cardH;
    }

    // ── Indicateur double ─────────────────────────────────────────────────────
    if (doubleStreak_[currentPlayer_] > 0 && state_ == State::PLAYING) {
        std::string ds = "Double x" + std::to_string(doubleStreak_[currentPlayer_])
                       + " — Rejouez !";
        window_.draw(makeText(font_, ds, 14, sf::Color(255, 220, 50),
                              px0 + 18.f, float(WIN_H) - 108.f));
    }

    // ── Prison indicator ──────────────────────────────────────────────────────
    if (!joueurs.empty()) {
        int cpt = joueurs[currentPlayer_]->getCptPrison();
        if (cpt > 0) {
            std::string ps = "En prison — tour " + std::to_string(cpt) + "/3";
            window_.draw(makeText(font_, ps, 14, sf::Color(255, 100, 100),
                                  px0 + 18.f, float(WIN_H) - 108.f));
        }
    }

    // ── Bouton "Lancer le dé" ─────────────────────────────────────────────────
    if (state_ == State::PLAYING) {
        float btnX = px0 + 60.f;
        float btnY = float(WIN_H) - 80.f;
        sf::Color base = PLAYER_COLORS[currentPlayer_];
        // Légèrement assombri pour les doubles (indique le re-roll automatique)
        sf::Color btnCol = (doubleStreak_[currentPlayer_] > 0)
                           ? sf::Color(base.r/2, base.g/2 + 80, base.b/2, 255)
                           : base;
        drawRect(window_, btnX, btnY, 300.f, 58.f, btnCol, sf::Color::Black, 2.f);
        std::string label = (doubleStreak_[currentPlayer_] > 0)
                            ? "Rejouer (double)" : "Lancer le d\xc3\xa9";
        window_.draw(makeText(font_, label, 22, sf::Color(16, 16, 16),
                              btnX + 18.f, btnY + 14.f));
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
//  prepareCarteDisplay()  – prépare titre, explication et texture de la carte
// ─────────────────────────────────────────────────────────────────────────────
void PlateauSFML::prepareCarteDisplay(Carte* card, Joueur* titulaire,
                                      int capDelta, int posBefore, int posAfter)
{
    hasCarteTexture_ = false;
    carteTitre_      = "Carte";
    carteExplication_ = "";
    if (!card || !titulaire) return;

    auto loadTex = [&](const std::string& path) {
        hasCarteTexture_ = carteTexture_.loadFromFile(path);
    };

    if (dynamic_cast<CarteAnniversaire*>(card)) {
        loadTex("assets/CarteAnniversaire.png");
        carteTitre_ = "Joyeux Anniversaire !";
        int nb = std::max(1, (int)joueurs.size() - 1);
        int parJoueur = (capDelta > 0) ? capDelta / nb : 0;
        carteExplication_ =
            "Chaque joueur verse " + std::to_string(parJoueur) + "\xe2\x82\xac"
            " \xc3\xa0 " + titulaire->getNom() + ".\n";
        for (auto* j2 : joueurs) {
            if (j2 && j2 != titulaire
                && j2->conditionfinanciere() != Condition::FAILLITE)
                carteExplication_ += j2->getNom() + " verse "
                    + std::to_string(parJoueur) + "\xe2\x82\xac" ".\n";
        }
        carteExplication_ += "Total re\xc3\xa7u : +" + std::to_string(capDelta)
            + "\xe2\x82\xac" ".";

    } else if (dynamic_cast<CarteArgent*>(card)) {
        // Pas d'image dédiée : fond coloré selon gain ou perte
        carteTitre_ = (capDelta >= 0) ? "Bonne nouvelle !" : "Mauvaise nouvelle !";
        if (capDelta >= 0)
            carteExplication_ = titulaire->getNom() + " re\xc3\xa7oit "
                + std::to_string(capDelta) + "\xe2\x82\xac" ".";
        else
            carteExplication_ = titulaire->getNom() + " perd "
                + std::to_string(-capDelta) + "\xe2\x82\xac" ".";

    } else if (dynamic_cast<CarteDeplacement*>(card)) {
        loadTex("assets/CarteDeplacement.png");
        carteTitre_ = "D\xc3\xa9placement !";
        if (posAfter != posBefore) {
            int nb = (int)cases.size();
            int steps = (posAfter - posBefore + nb) % nb;
            carteExplication_ = titulaire->getNom() + " avance de "
                + std::to_string(steps) + " case"
                + (steps > 1 ? "s" : "") + "\n"
                + "et arrive sur : "
                + (posAfter < (int)cfg_.cases.size()
                   ? cfg_.cases[posAfter].name : "?") + ".";
        } else {
            carteExplication_ = titulaire->getNom()
                + " se d\xc3\xa9place (aucun changement).";
        }

    } else if (dynamic_cast<CarteDoubleGain*>(card)) {
        loadTex("assets/CarteDoubleGain.png");
        carteTitre_ = "Double Gain !";
        carteExplication_ =
            titulaire->getNom() + " obtient la carte Double Gain.\n\n"
            "Avant un jeu de propri\xc3\xa9t\xc3\xa9, vous\n"
            "pouvez l'activer pour doubler vos gains\n"
            "en cas de victoire !\n"
            "(consomm\xc3\xa9" "e apr\xc3\xa8s utilisation)";

    } else if (dynamic_cast<CarteSortiePrison*>(card)) {
        loadTex("assets/CarteSortiePrison.png");
        carteTitre_ = "Sortie de Prison !";
        carteExplication_ =
            titulaire->getNom() + " obtient la carte Sortie de Prison.\n\n"
            "En prison, utilisez-la avant de lancer\n"
            "les d\xc3\xa9s pour en sortir imm\xc3\xa9" "diatement !";
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  renderCarteDrawnPrompt()  – overlay affiché après un tirage de carte
// ─────────────────────────────────────────────────────────────────────────────
void PlateauSFML::renderCarteDrawnPrompt()
{
    if (!hasFont_) return;

    // Dim
    sf::RectangleShape dim{sf::Vector2f(float(WIN_W), float(WIN_H))};
    dim.setFillColor(sf::Color(0, 0, 0, 180));
    window_.draw(dim);

    float cx = WIN_W / 2.f;
    float cy = WIN_H / 2.f;
    float pw = 700.f, ph = 370.f;
    float px = cx - pw / 2.f, py = cy - ph / 2.f;

    // Panel background
    drawRect(window_, px, py, pw, ph,
             sf::Color(18, 18, 34), sf::Color(220, 180, 0), 2.f);

    // ── Header ───────────────────────────────────────────────────────────────
    window_.draw(makeText(font_, "Carte pioch\xc3\xa9" "e !", 18,
                          sf::Color(180, 180, 180), px + 14, py + 8));
    // Title
    sf::Text title = makeText(font_, carteTitre_, 24,
                              sf::Color(240, 230, 80), 0, 0);
    title.setPosition(px + 14, py + 32);
    window_.draw(title);

    // ── Card image (left zone) ────────────────────────────────────────────────
    float imgX = px + 14.f, imgY = py + 70.f;
    float imgW = 190.f,     imgH = 255.f;

    if (hasCarteTexture_) {
        sf::Sprite sp(carteTexture_);
        sp.setScale(imgW / carteTexture_.getSize().x,
                    imgH / carteTexture_.getSize().y);
        sp.setPosition(imgX, imgY);
        window_.draw(sp);
    } else {
        // Colored fallback for cards with no image
        sf::Color fc = (carteTitre_.find("Bonne") != std::string::npos)
                       ? sf::Color(30, 80, 30) : sf::Color(80, 30, 30);
        drawRect(window_, imgX, imgY, imgW, imgH, fc,
                 sf::Color(180, 180, 180), 1.f);
        sf::Text q = makeText(font_, "?", 60, sf::Color(180, 180, 180),
                              imgX + imgW/2 - 20, imgY + imgH/2 - 35);
        window_.draw(q);
    }

    // ── Explanation text (right zone) ────────────────────────────────────────
    float tx   = imgX + imgW + 18.f;
    float tMaxW = pw - imgW - 50.f;   // available width (unused for wrapping)
    float lineY = py + 70.f;
    float lineH = 26.f;

    std::string expl = carteExplication_;
    std::string::size_type pos = 0;
    while (true) {
        auto nl = expl.find('\n', pos);
        std::string line = expl.substr(pos,
            nl == std::string::npos ? std::string::npos : nl - pos);
        if (!line.empty())
            window_.draw(makeText(font_, line, 15,
                                  sf::Color(215, 215, 215), tx, lineY));
        lineY += lineH;
        if (nl == std::string::npos) break;
        pos = nl + 1;
    }

    // ── Continuer button ─────────────────────────────────────────────────────
    float btnX = cx - 90.f, btnY = py + ph - 68.f;
    drawRect(window_, btnX, btnY, 180.f, 50.f,
             sf::Color(50, 100, 170), sf::Color::White, 2.f);
    window_.draw(makeText(font_, "Continuer", 20,
                          sf::Color::White, btnX + 22.f, btnY + 13.f));
    window_.draw(makeText(font_, "[Espace / Entr\xc3\xa9" "e]", 12,
                          sf::Color(160, 160, 160),
                          btnX + 12.f, btnY + 34.f));
}

// ─────────────────────────────────────────────────────────────────────────────
//  renderUseSortiePrisonPrompt()
// ─────────────────────────────────────────────────────────────────────────────
void PlateauSFML::renderUseSortiePrisonPrompt()
{
    if (!hasFont_) return;

    sf::RectangleShape dim{sf::Vector2f(float(WIN_W), float(WIN_H))};
    dim.setFillColor(sf::Color(0, 0, 0, 165));
    window_.draw(dim);

    float cx = WIN_W / 2.f;
    drawRect(window_, cx - 310, 290, 620, 230,
             sf::Color(24, 24, 38), sf::Color(220, 180, 0), 2.f);

    window_.draw(makeText(font_, "Carte Sortie de Prison disponible !",
                          22, sf::Color(220, 180, 0), cx - 290, 308));

    if (!joueurs.empty()) {
        window_.draw(makeText(font_,
            joueurs[currentPlayer_]->getNom()
            + " est en prison (tour "
            + std::to_string(joueurs[currentPlayer_]->getCptPrison()) + "/3)",
            16, sf::Color(200, 200, 200), cx - 290, 345));
    }
    window_.draw(makeText(font_,
        "Utiliser la carte pour sortir imm\xc3\xa9" "diatement ?",
        16, sf::Color(185, 185, 185), cx - 290, 375));

    // "Utiliser" button
    drawRect(window_, cx - 270.f, 430.f, 220.f, 50.f,
             sf::Color(45, 145, 45), sf::Color::Black, 2.f);
    window_.draw(makeText(font_, "Utiliser la carte", 17,
                          sf::Color::White, cx - 255.f, 443.f));

    // "Jouer normalement" button
    drawRect(window_, cx + 50.f, 430.f, 220.f, 50.f,
             sf::Color(100, 80, 30), sf::Color::Black, 2.f);
    window_.draw(makeText(font_, "Lancer les d\xc3\xa9s", 17,
                          sf::Color::White, cx + 65.f, 443.f));
}

// ─────────────────────────────────────────────────────────────────────────────
//  renderUseDoubleGainPrompt()
// ─────────────────────────────────────────────────────────────────────────────
void PlateauSFML::renderUseDoubleGainPrompt()
{
    if (!hasFont_ || !pendingGamble_) return;

    sf::RectangleShape dim{sf::Vector2f(float(WIN_W), float(WIN_H))};
    dim.setFillColor(sf::Color(0, 0, 0, 165));
    window_.draw(dim);

    float cx = WIN_W / 2.f;
    drawRect(window_, cx - 310, 270, 620, 260,
             sf::Color(24, 24, 38), sf::Color(220, 180, 0), 2.f);

    window_.draw(makeText(font_, "Carte Double Gain disponible !",
                          22, sf::Color(220, 180, 0), cx - 290, 288));

    window_.draw(makeText(font_,
        "Mise choisie : " + std::to_string(miseSelectionnee_) + " \xe2\x82\xac",
        17, sf::Color(200, 200, 200), cx - 290, 328));
    window_.draw(makeText(font_,
        "Si vous gagnez : +" + std::to_string(miseSelectionnee_ * 2)
        + " \xe2\x82\xac au lieu de +" + std::to_string(miseSelectionnee_) + " \xe2\x82\xac !",
        16, sf::Color(80, 220, 80), cx - 290, 358));
    window_.draw(makeText(font_,
        "La carte est consomm\xc3\xa9" "e apr\xc3\xa8s utilisation.",
        14, sf::Color(160, 160, 160), cx - 290, 390));

    // "Utiliser" button
    drawRect(window_, cx - 270.f, 430.f, 220.f, 50.f,
             sf::Color(45, 145, 45), sf::Color::Black, 2.f);
    window_.draw(makeText(font_, "Utiliser (x2 gain)", 17,
                          sf::Color::White, cx - 258.f, 443.f));

    // "Ne pas utiliser" button
    drawRect(window_, cx + 50.f, 430.f, 220.f, 50.f,
             sf::Color(100, 50, 50), sf::Color::Black, 2.f);
    window_.draw(makeText(font_, "Jouer normalement", 17,
                          sf::Color::White, cx + 58.f, 443.f));
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
