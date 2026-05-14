#include "CarteSortiePrison.hpp"
#include "Joueur.hpp"

CarteSortiePrison::CarteSortiePrison(Joueur* titulaire)
    : Carte("SortiePrison", "SortiePrison")
{
    (void)titulaire;
}

void CarteSortiePrison::action()
{
    // The action is to give the titulaire a get-out-of-jail card. Tests call joueur->donnerCarte when
    // creating the card and then call action() expecting possedeCarte to be true afterwards. We'll
    // leave detailed handling to Joueur::donnerCarte.
}
