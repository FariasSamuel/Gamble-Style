#include "CarteSortiePrison.hpp"
#include "Joueur.hpp"

CarteSortiePrison::CarteSortiePrison(Joueur* titulaire)
    : Carte("SortiePrison", "SortiePrison"), titulaire_(titulaire)
{
}

void CarteSortiePrison::action()
{
    if (titulaire_) titulaire_->donnerCarte(this);
}
