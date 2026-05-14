#include "CarteAnniversaire.hpp"
#include "Joueur.hpp"
#include "Plateau.hpp"

CarteAnniversaire::CarteAnniversaire(Joueur* titulaire, Plateau* plateau, int montant)
    : Carte("Anniversaire", "Anniversaire"),
      titulaire_(titulaire), plateau_(plateau), montant_(montant)
{
}

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
