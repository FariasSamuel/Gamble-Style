// Joueur.hpp
// Représente un joueur dans la partie : capital, propriétés, cartes, position
// sur le plateau et état financier (RICHE / BANQUEROUTE / FAILLITE).
//
// Le joueur ne possède pas les objets Case, CasePropriete ou Carte au sens
// RAII : ces objets appartiennent au Plateau. Le joueur stocke uniquement
// des pointeurs observateurs.

#ifndef JOUEUR_H
#define JOUEUR_H

#include <string>
#include <vector>
#include "Case.hpp"

class Case;
class CasePropriete;
class Carte;
class CarteSortiePrison;
class CarteDoubleGain;

// État financier d'un joueur :
//   RICHE       — capital positif, peut continuer à jouer
//   BANQUEROUTE — capital ≤ 0 mais pas encore déclaré en faillite
//   FAILLITE    — misebanqueroute() appelé et actifs liquidés ; joueur éliminé
enum class Condition {
    RICHE,
    BANQUEROUTE,
    FAILLITE
};

// Joueur — unité de jeu identifiée par un nom et dotée d'un capital.
// Un joueur peut acheter des propriétés, tirer des cartes et être emprisonné.
class Joueur {
public:
    Joueur(const std::string& name, int capital_init = 1500);
    ~Joueur();

    // ── Identité ────────────────────────────────────────────────────────────

    // Retourne le nom du joueur (immuable après construction).
    const std::string& getNom() const;

    // ── Capital ─────────────────────────────────────────────────────────────

    int  getCapital() const;
    void setCapital(int c);

    // ── Dés & déplacement ──────────────────────────────────────────────────

    // Lance deux dés, stocke le résultat et retourne la somme.
    int  lancerde();

    // Retourne le résultat du dernier lancer (somme des deux dés).
    int  getDernierLancer() const;

    // Déplace le joueur sur la case c (met à jour position).
    void bougerjoueur(Case* c);

    Case* getCaseActuelle() const;
    void  setCaseActuelle(Case* c);

    // ── Propriétés ─────────────────────────────────────────────────────────

    // Achète la propriété cp (déduit le prix du capital, ajoute à la liste).
    void acheter(CasePropriete* cp);

    int  getNbProprietes()  const;  // nombre de propriétés détenues
    int  getValPropriete()  const;  // valeur totale (somme des prix d'achat)

    // Hypothèque cp : perçoit la moitié du prix, retire cp de la liste.
    void hypotequer(CasePropriete* cp);

    // Vide la liste de propriétés (utilisé lors d'un reset de partie).
    void clearProprietes();

    // Liquide toutes les propriétés à mi-prix ; passe en FAILLITE si capital ≤ 0.
    void misebanqueroute();

    // Retourne l'état financier courant du joueur.
    Condition conditionfinanciere() const;

    // ── Cartes ──────────────────────────────────────────────────────────────

    // Ajoute une carte à l'inventaire du joueur.
    void donnerCarte(Carte* c);

    // Retourne true si le joueur possède la carte identifiée par id (Carte::ID).
    bool possedeCarte(int id) const;

    // Retire une carte précise de l'inventaire (sans la détruire).
    void retirerCarte(Carte* c);

    // Raccourcis : retournent la première carte du type demandé, ou nullptr.
    CarteSortiePrison* getSortiePrison() const;
    CarteDoubleGain*   getDoubleGain()   const;

    // ── Tour de jeu ─────────────────────────────────────────────────────────

    // Joue un tour complet : lancer les dés, gérer les doubles, déplacer.
    // Utilisé par Plateau::update() en mode console.
    void tour(const std::vector<Case*>& cases, int prisonIdx = -1);

    // ── Doubles ─────────────────────────────────────────────────────────────

    // Retourne le nombre de doubles consécutifs lancés lors de ce tour.
    int getCompteurDouble() const;

    // ── Prison ──────────────────────────────────────────────────────────────

    // Compteur de tours restants en prison (0 = libre, max = 3).
    int  getCptPrison() const;
    void setCptPrison(int v);

private:
    const std::string         nom;
    int                       capital;
    Condition                 condition_;       // état financier mémorisé (RICHE par défaut)
    Case*                     position;         // case actuelle (non propriétaire)
    std::vector<CasePropriete*> proprietes;     // propriétés achetées (non propriétaire)
    std::vector<Carte*>       cartes;           // cartes en main (non propriétaire)
    int                       compteur_double;  // doubles consécutifs ce tour
    int                       lastLancer{0};    // résultat du dernier lancer de dés
    int                       cpt_prison_{0};   // tours restants en prison
};

#endif // JOUEUR_H
