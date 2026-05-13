#ifndef CASEPROPRIETE_H
#define CASEPROPRIETE_H

#include "Case.hpp"

class CasePropriete : public Case {
public:
    explicit CasePropriete(int num = 0);
    ~CasePropriete() override;

    void action() override;

private:
    int prix_achat;
    int prix_hypotheque;
    bool achetable;
};

#endif // CASEPROPRIETE_H