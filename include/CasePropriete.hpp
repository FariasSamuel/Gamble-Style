// CasePropriete.hpp
// Case pouvant être achetée par un joueur. Si un autre joueur atterrit dessus,
// un mini-jeu (Tron par défaut) est lancé pour déterminer qui remporte la mise.
//
// Cycle d'une case propriété occupée :
//   1. action()        — détecte visiteur, pose les flags d'état
//   2. PlateauSFML     — affiche le prompt "Acheter ?" ou "Choisir mise"
//   3. confirmerMise() — lance le sous-processus mini-jeu, appelle repartition()

#ifndef CASEPROPRIETE_H
#define CASEPROPRIETE_H

#include "Case.hpp"
#include <string>

class Plateau;
class Joueur;

// CasePropriete — case achetable avec un prix d'achat et une logique de jeu 1v1.
class CasePropriete : public Case {
public:
    explicit CasePropriete(int num = 0);
    CasePropriete(const std::string& name, int prix);
    CasePropriete(int num, const std::string& name, int prix);
    ~CasePropriete() override;

    // Évalue la situation (non achetée → flag achat ; propriétaire ≠ visiteur → flag gamble).
    void action() override;

    // ── Getters de base ──────────────────────────────────────────────────────

    int                getPrix()  const;   // prix d'achat (€)
    const std::string& getName()  const;   // nom affiché dans l'UI

    // ── Propriétaire ────────────────────────────────────────────────────────

    void    setProprietaire(Joueur* j);
    Joueur* getProprietaire() const;

    // Surcharge de Case::setJoueurActif pour réinitialiser les flags à chaque visite.
    void setJoueurActif(Joueur* j);

    // Marque la case comme achetable ou non (utilisé lors de la banqueroute).
    void setAchetable(bool val);

    // ── Câblage depuis buildGame() ───────────────────────────────────────────

    // Commande shell du mini-jeu (ex : "./bin/Tron").
    void setCommand(const std::string& cmd);

    // Référence au plateau, nécessaire pour récupérer la liste des joueurs.
    void setPlateau(Plateau* p);

    // ── Flags d'état interrogés par PlateauSFML après action() ──────────────

    bool achatPropose()    const;  // true → case non achetée, proposer l'achat
    bool gambleEnAttente() const;  // true → visiteur sur case adverse, choisir la mise
    bool minijeuLance()    const;  // true → jeu en cours (après confirmerMise)

    // ── Résolution du jeu ───────────────────────────────────────────────────

    // Lance le mini-jeu pour la mise choisie, puis appelle repartition().
    void confirmerMise(int mise);

    // Effectue le transfert de capital selon le résultat (nullptr = égalité).
    void repartition(Joueur* gagnant, int mise);

    // Gagnant du dernier jeu (nullptr si égalité ou pas encore joué).
    Joueur* getGagnantDernierJeu() const;

private:
    int         prix_achat;           // prix pour devenir propriétaire
    int         prix_hypotheque;      // valeur de remboursement (prix / 2)
    bool        achetable;            // false si la case est bloquée (banqueroute)
    std::string name_;                // nom de la case
    Joueur*     proprietaire;         // propriétaire actuel (nullptr si libre)
    std::string command_;             // commande du mini-jeu
    Plateau*    plateau_;             // pointeur vers le plateau (pour les joueurs)

    bool    flag_achat_propose;       // mis à true si visiteur peut acheter
    bool    flag_gamble_attente;      // mis à true si un duel est à lancer
    bool    flag_minijeu_lance;       // mis à true après confirmerMise()
    Joueur* dernierGagnant_{nullptr}; // résultat du dernier jeu joué
};

#endif // CASEPROPRIETE_H
