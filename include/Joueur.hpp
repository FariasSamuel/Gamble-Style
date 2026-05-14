/**
 * @file Joueur.hpp
 * @brief Déclaration de la classe Joueur.
 * @project GambleStyle — GM4 INSA Rouen Normandie
 */
#ifndef JOUEUR_HPP
#define JOUEUR_HPP

#include <string>
#include <vector>
#include "Condition.hpp"

class CasePropriete;
class Case;
class Carte;

/**
 * @class Joueur
 * @brief Représente un participant au jeu GambleStyle.
 */
class Joueur {
public:
    Joueur(const std::string& nom, int capital);
    virtual ~Joueur() = default;

    // ── Accesseurs ────────────────────────────────────────────
    const std::string& getNom()        const;
    int  getCapital()                  const;
    int  getValPropriete()             const;
    int  getNbProprietes()             const;
    Case* getCaseActuelle()            const;
    int  getDernierLancer()            const;
    int  getCompteurTours()            const;
    int  getNbCartes()                 const;
    int getPosition() const;
    const std::vector<CasePropriete*>& getProprietes() const;

    // ── Modificateurs ─────────────────────────────────────────
    void setCapital(int c);
    void setDernierLancer(int v);
    void resetCompteurTours();

    // ── Actions financières ───────────────────────────────────
    void donnerCapital(int montant);
    void enleverCapital(int montant);
    Condition conditionfinanciere()    const;
    int  capitalSolvabilite()          const;
    void misebanqueroute();

    // ── Propriétés ────────────────────────────────────────────
    void acheter(CasePropriete* caseProp);
    void hypotequer(CasePropriete* caseProp);
    void clearProprietes();
    /** @brief Utilisé par CarteImpotMinijeux dans les tests. */
    void setNbProprietesMock(int n);

    // ── Cartes ────────────────────────────────────────────────
    void donnerCarte(Carte* carte);
    bool possedeCarte(int id)          const;
    void retirerCarte(int id);

    // ── Déplacement et tour ───────────────────────────────────
    int  lancerde();
    virtual void tour();
    void bougerjoueur(Case* dest);
    void setPosition(int index);

protected:
    std::string nom;
    int capital;
    int val_propriete;
    int position;
    int dernier_lancer;
    int compteur_double;
    int compteur_tours;
    int nb_proprietes_mock;          ///< Pour CarteImpotMinijeux tests
    bool use_mock_nb;
    std::vector<Carte*> cartes;
    std::vector<CasePropriete*> proprietes;
    Case* case_actuelle;
};

#endif // JOUEUR_HPP
