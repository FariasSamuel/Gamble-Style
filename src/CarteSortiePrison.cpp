// CarteSortiePrison.cpp
// Carte conservée en main permettant de sortir de prison sans lancer de double.
// action() place la carte dans l'inventaire du titulaire ; PlateauSFML la retire
// via Joueur::retirerCarte() une fois utilisée.

#include "CarteSortiePrison.hpp"
#include "Joueur.hpp"

CarteSortiePrison::CarteSortiePrison(Joueur* titulaire)
    : Carte("SortiePrison", "Sortie de Prison !"), titulaire_(titulaire)
{
}

// Place la carte dans l'inventaire du titulaire.
void CarteSortiePrison::action()
{
    if (titulaire_) titulaire_->donnerCarte(this);
}
