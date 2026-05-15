// CarteAnniversaire.cpp
// Chaque adversaire vivant verse montant_ € au titulaire.
// plateau_ est nécessaire pour itérer sur tous les joueurs de la partie.

#include "CarteAnniversaire.hpp"
#include "Joueur.hpp"
#include "Plateau.hpp"

CarteAnniversaire::CarteAnniversaire(Joueur* titulaire, Plateau* plateau, int montant)
    : Carte("Anniversaire", "Joyeux Anniversaire !"),
      titulaire_(titulaire), plateau_(plateau), montant_(montant)
{
}

// Prélève montant_ € sur chaque joueur différent du titulaire et les verse au titulaire.
void CarteAnniversaire::action()
{
    if (!titulaire_ || !plateau_) return;
    for (int i = 0; i < plateau_->nb_joueur(); ++i) {
        Joueur* j = plateau_->getJoueur(i);
        if (j && j != titulaire_) {
            j->setCapital(j->getCapital() - montant_);
            titulaire_->setCapital(titulaire_->getCapital() + montant_);
        }
    }
}
