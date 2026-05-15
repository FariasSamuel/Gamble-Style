#include "CasePropriete.hpp"
#include "Joueur.hpp"
#include "Plateau.hpp"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <algorithm>

// ── Constructeurs ─────────────────────────────────────────────────────────────

CasePropriete::CasePropriete(int num)
    : Case(num), prix_achat(0), prix_hypotheque(0), achetable(true), name_(""),
      proprietaire(nullptr), command_(""), plateau_(nullptr),
      flag_achat_propose(false), flag_gamble_attente(false), flag_minijeu_lance(false)
{}

CasePropriete::CasePropriete(const std::string& name, int prix)
    : Case(0), prix_achat(prix), prix_hypotheque(prix / 2), achetable(true), name_(name),
      proprietaire(nullptr), command_(""), plateau_(nullptr),
      flag_achat_propose(false), flag_gamble_attente(false), flag_minijeu_lance(false)
{}

CasePropriete::CasePropriete(int num, const std::string& name, int prix)
    : Case(num), prix_achat(prix), prix_hypotheque(prix / 2), achetable(true), name_(name),
      proprietaire(nullptr), command_(""), plateau_(nullptr),
      flag_achat_propose(false), flag_gamble_attente(false), flag_minijeu_lance(false)
{}

CasePropriete::~CasePropriete() = default;

// ── Lancement du jeu externe ──────────────────────────────────────────────────
// Retourne le gagnant parmi j1/j2, ou nullptr en cas d'égalité / erreur.
static Joueur* lancerJeu(const std::string& cmd, Joueur* j1, Joueur* j2)
{
    if (cmd.empty() || !j1 || !j2)
        return (std::rand() % 2) ? j1 : j2;

    std::string full = cmd + " \"" + j1->getNom() + "\" \"" + j2->getNom() + "\"";
    FILE* pipe = popen(full.c_str(), "r");
    if (!pipe) return (std::rand() % 2) ? j1 : j2;

    char buf[256] = {};
    Joueur* result = nullptr;
    if (fgets(buf, sizeof(buf), pipe)) {
        std::string nom(buf);
        nom.erase(std::remove(nom.begin(), nom.end(), '\n'), nom.end());
        nom.erase(std::remove(nom.begin(), nom.end(), '\r'), nom.end());
        if (j1->getNom() == nom)      result = j1;
        else if (j2->getNom() == nom) result = j2;
        // sinon nullptr = égalité
    }
    pclose(pipe);
    return result;
}

// ── action() ─────────────────────────────────────────────────────────────────
void CasePropriete::action()
{
    flag_achat_propose   = false;
    flag_gamble_attente  = false;
    flag_minijeu_lance   = false;

    Joueur* visiteur = getJoueurActif();

    if (!proprietaire) {
        flag_achat_propose = true;
        return;
    }

    if (visiteur == proprietaire) return;

    // Propriété d'un autre joueur : le propriétaire choisit sa mise via l'UI
    flag_gamble_attente = true;
}

// ── confirmerMise() ───────────────────────────────────────────────────────────
void CasePropriete::confirmerMise(int mise)
{
    mise = std::clamp(mise, 50, 200);
    Joueur* visiteur = getJoueurActif();
    Joueur* gagnant  = lancerJeu(command_, visiteur, proprietaire);
    flag_minijeu_lance = true;
    repartition(gagnant, mise);
}

// ── repartition() ─────────────────────────────────────────────────────────────
void CasePropriete::repartition(Joueur* gagnant, int mise)
{
    if (!gagnant) return; // égalité : rien ne se passe

    Joueur* visiteur = getJoueurActif();
    if (!visiteur || !proprietaire) return;

    Joueur* perdant = (gagnant == proprietaire) ? visiteur : proprietaire;

    if (plateau_) {
        plateau_->transfert_argent(perdant, gagnant, mise);
    } else {
        // Fallback utilisé par les tests (pas de plateau injecté)
        perdant->setCapital(perdant->getCapital() - mise);
        gagnant->setCapital(gagnant->getCapital() + mise);
    }
}

// ── Accesseurs ────────────────────────────────────────────────────────────────
int                CasePropriete::getPrix()          const { return prix_achat; }
const std::string& CasePropriete::getName()          const { return name_; }
void               CasePropriete::setProprietaire(Joueur* j) { proprietaire = j; }
Joueur*            CasePropriete::getProprietaire()  const { return proprietaire; }
void               CasePropriete::setJoueurActif(Joueur* j) { Case::setJoueurActif(j); }
void               CasePropriete::setAchetable(bool val)    { achetable = val; }
void               CasePropriete::setCommand(const std::string& cmd) { command_ = cmd; }
void               CasePropriete::setPlateau(Plateau* p)    { plateau_ = p; }

bool CasePropriete::achatPropose()    const { return flag_achat_propose; }
bool CasePropriete::gambleEnAttente() const { return flag_gamble_attente; }
bool CasePropriete::minijeuLance()    const { return flag_minijeu_lance; }
