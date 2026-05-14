#include "CarteAnniversaire.hpp"
#include "Joueur.hpp"
#include "Plateau.hpp"

CarteAnniversaire::CarteAnniversaire(Joueur* titulaire, Plateau* plateau, int montant)
    : Carte("Anniversaire", "Anniversaire")
{
    (void)titulaire; (void)plateau; (void)montant;
}

void CarteAnniversaire::action()
{
    // Minimal behavior for tests: if plateau and titulaire known, transfer will be handled externally
}
