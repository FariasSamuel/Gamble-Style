// CasePrison.hpp
// Case "Station Prison" : les joueurs envoyés ici par CaseGotoPrison doivent
// lancer un double pour sortir, ou utiliser une CarteSortiePrison.
// Après 3 tours sans double, le joueur est libéré automatiquement.

#ifndef CASEPRISON_H
#define CASEPRISON_H

#include "Case.hpp"
#include <map>

class Joueur;

// CasePrison — gère l'emprisonnement par joueur via une map de compteurs.
// action() est appelé à chaque tour du joueur emprisonné.
class CasePrison : public Case {
public:
    explicit CasePrison(int num = 0);
    ~CasePrison() override;

    // Tente de libérer le joueur actif : double → libéré, sinon compteur++.
    // Après 3 tours sans double le joueur est automatiquement libéré.
    void action() override;

    // ── Helpers pour les tests ───────────────────────────────────────────────

    // Active ou désactive le recours à une carte Sortie de Prison (mock de décision UI).
    void setUtiliserCarteReponse(bool val);

    // Fixe un résultat de dés déterministe pour les tests (persistant jusqu'à reset).
    void forcerResultatDes(int d1, int d2);

    // Retourne le nombre de tours qu'un joueur a passés en prison.
    int getTourPrison(Joueur* j);

private:
    std::map<Joueur*, int> tours_;          // compteur de tours par joueur emprisonné
    bool utiliserCarteReponse_;             // mock : simuler l'utilisation d'une carte
    int  forced_d1_;                        // dé 1 forcé (tests)
    int  forced_d2_;                        // dé 2 forcé (tests)
    bool has_forced_;                       // true si le résultat est forcé
};

#endif // CASEPRISON_H
