#ifndef CASEDEPART_H
#define CASEDEPART_H

#include "Case.hpp"

class CaseDepart : public Case {
public:
    explicit CaseDepart(int num = 0, int montant = 0);
    
    
    ~CaseDepart() override;

    void action() override;

    int getMontant() const;
    void setJoueurActif(Joueur* j);

private:
    int montant;
};

#endif // CASEDEPART_H