#ifndef CASEPRISON_H
#define CASEPRISON_H

#include "Case.hpp"
#include <map>

class Joueur;

class CasePrison : public Case {
public:
    explicit CasePrison(int num = 0);
    ~CasePrison() override;

    void action() override;

    // test helpers
    void setUtiliserCarteReponse(bool val);
    void forcerResultatDes(int d1, int d2);
    int getTourPrison(Joueur* j);

private:
    std::map<Joueur*, int> tours_;
    bool utiliserCarteReponse_;
    int forced_d1_;
    int forced_d2_;
    bool has_forced_;
};

#endif // CASEPRISON_H