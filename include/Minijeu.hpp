#ifndef MINIJEU_H
#define MINIJEU_H

#include <cstdlib>

class Joueur;

class Minijeu {
public:
    virtual ~Minijeu() {}
    virtual Joueur* jouer(Joueur* a, Joueur* b) { return (std::rand()%2==0)?a:b; }
    virtual Joueur** jouer(Joueur* a, Joueur* b, Joueur* c) { return nullptr; }
    virtual Joueur** jouer(Joueur* a, Joueur* b, Joueur* c, Joueur* d) { return nullptr; }
};

#endif // MINIJEU_H
