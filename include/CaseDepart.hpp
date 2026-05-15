// CaseDepart.hpp
// Case spéciale de départ du plateau. Chaque fois qu'un joueur passe ou
// atterrit sur cette case, il reçoit un bonus en capital (défini dans board.txt).

#ifndef CASEDEPART_H
#define CASEDEPART_H

#include "Case.hpp"

class Joueur;

// CaseDepart — case index 0, versant un bonus au joueur qui la traverse.
// L'attribution du bonus est gérée côté PlateauSFML (détection de passage)
// plutôt que dans action(), car un simple passage ne déclenche pas action().
class CaseDepart : public Case {
public:
    // montant : bonus versé à chaque passage (€) ; num : index sur le plateau.
    explicit CaseDepart(int montant = 0, int num = 0);
    ~CaseDepart() override;

    // Versement du bonus au joueur actif lorsqu'il atterrit sur le départ.
    void action() override;

    // Retourne le montant du bonus de passage.
    int getMontant() const;

    // Définit le joueur actif avant d'appeler action().
    void setJoueurActif(Joueur* j);

private:
    int montant;  // bonus versé en euros à chaque passage/atterrissage
};

#endif // CASEDEPART_H
