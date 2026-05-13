#ifndef CASE_H
#define CASE_H

#include <string>

class Joueur;

class Case {
public:
    explicit Case(int num = 0);
    virtual ~Case();

    int get_num_case() const;
    virtual void action();

private:
    int num_case;
};

#endif // CASE_H