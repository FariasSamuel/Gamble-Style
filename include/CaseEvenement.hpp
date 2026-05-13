#ifndef CASEEVENEMENT_H
#define CASEEVENEMENT_H

#include "Case.hpp"

class CaseEvenement : public Case {
public:
    explicit CaseEvenement(int num = 0);
    ~CaseEvenement() override;

    void action() override;

private:
    int montant;
};

#endif // CASEEVENEMENT_H