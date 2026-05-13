#ifndef CASEPRISON_H
#define CASEPRISON_H

#include "Case.hpp"

class CasePrison : public Case {
public:
    explicit CasePrison(int num = 0);
    ~CasePrison() override;

    void action() override;

private:
    int liste_tour[4];
};

#endif // CASEPRISON_H