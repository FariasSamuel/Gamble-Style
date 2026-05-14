/**
 * @file CasePropriete.hpp
 * @brief Classe CasePropriete.
 * @project GambleStyle — GM4 INSA Rouen Normandie
 */
#ifndef CASEPROPRIETE_HPP
#define CASEPROPRIETE_HPP

#include "Case.hpp"
#include <string>

// ═══════════════════════════════════════════════════════════════
// CasePropriete
// ═══════════════════════════════════════════════════════════════
/**
 * @class CasePropriete
 * @brief Case achetable avec système de duel (gamble).
 */
class CasePropriete : public Case {
public:
    CasePropriete(const std::string& nom, int prix);
    void action() override;

    // Accesseurs
    Joueur* getProprietaire()  const;
    int getPrix()              const;
    int getPrixHypotheque()    const;
    bool isAchetable()         const;
    bool achatPropose()        const;
    bool minijeuLance()        const;
    int getGamble()            const;

    // Modificateurs
    void setProprietaire(Joueur* j);
    void setAchetable(bool v);
    void setGamble(int montant);
    void setGagnant(Joueur* j);
    void setGamblePredefini(int v);
    void setMiseLimites(int min, int max);

    // Logique
    void gamble();
    void gamble(Joueur* visiteur); // Surcharge ajoutée selon ton implémentation
    void repartition();

private:
    int prix_achat;
    int prix_hypotheque;
    Joueur* proprietaire;
    bool achetable;
    bool achat_propose;
    bool minijeu_lance;
    int gamble_montant;
    int gamble_predefini;
    int mise_min;
    int mise_max;
    Joueur* gagnant;
};

#endif // CASEPROPRIETE_HPP
