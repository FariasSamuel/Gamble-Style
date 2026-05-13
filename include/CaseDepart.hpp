#ifndef CASEDEPART_H
#define CASEDEPART_H

#include "Case.hpp"

class CaseDepart : public Case {
public:
    explicit CaseDepart(int num = 0, int montant = 0);
    ~CaseDepart() override;

    void action() override;

private:
    int montant;
};

#endif // CASEDEPART_H