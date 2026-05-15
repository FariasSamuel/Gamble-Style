// CaseGotoPrison.hpp
// Case "Aller en Prison" : téléporte immédiatement le joueur actif vers la
// CasePrison et initialise son compteur de tours (setCptPrison(1)).

#ifndef CASEGOTOPRISON_H
#define CASEGOTOPRISON_H

#include "Case.hpp"

class CasePrison;
class Joueur;

// CaseGotoPrison — case de transit ; elle-même n'emprisonne pas (le joueur
// ne fait que passer), c'est action() qui effectue la téléportation vers prison_.
class CaseGotoPrison : public Case {
public:
    explicit CaseGotoPrison(int num = 0, CasePrison* prison = nullptr);
    ~CaseGotoPrison() override;

    // Téléporte le joueur actif sur la case prison_ et démarre le compteur prison.
    void action() override;

    // Permet de câbler la case prison après construction (utilisé par buildGame()).
    void setPrisonCase(CasePrison* p);

private:
    CasePrison* prison_;  // case Prison cible (non propriétaire)
};

#endif // CASEGOTOPRISON_H
