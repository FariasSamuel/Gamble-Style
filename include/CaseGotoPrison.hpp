#ifndef CASEGOTOPRISON_H
#define CASEGOTOPRISON_H

#include "Case.hpp"

class CasePrison;
class Joueur;

class CaseGotoPrison : public Case {
public:
    explicit CaseGotoPrison(int num = 0, CasePrison* prison = nullptr);
    ~CaseGotoPrison() override;

    // Teleports the active player to the prison case.
    void action() override;

    void setPrisonCase(CasePrison* p);

private:
    CasePrison* prison_;
};

#endif // CASEGOTOPRISON_H
