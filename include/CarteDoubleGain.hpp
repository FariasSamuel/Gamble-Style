// CarteDoubleGain.hpp
// Carte conservée en main. Si le joueur l'active avant un duel de propriété,
// il reçoit le double de la mise en cas de victoire.
// La carte est consommée après usage (consommer() la désactive).

#ifndef CARTEDOUBLEGAIN_H
#define CARTEDOUBLEGAIN_H

#include "Carte.hpp"

class Joueur;

// CarteDoubleGain — carte d'inventaire à usage unique.
// ID statique utilisé par Joueur::possedeCarte() pour vérifier si le joueur
// possède ce type de carte.
class CarteDoubleGain : public Carte {
public:
    static const int ID = 2;  // identifiant unique pour possedeCarte()

    CarteDoubleGain(const std::string& nom = "", const std::string& text = "");
    ~CarteDoubleGain() override;

    // Remet la carte dans l'inventaire du titulaire (active_ = true).
    void action() override;

    // Mis à jour par CaseAleatoire::action() avant chaque tirage.
    void setTitulaire(Joueur* j) override { titulaire_ = j; }

    // Retourne le propriétaire actuel de la carte.
    Joueur* getTitulaire() const;

    // Retourne true si la carte est active (prête à être utilisée).
    bool estActive() const;

    // Désactive la carte après usage (ne la détruit pas — la case en est propriétaire).
    void consommer();

private:
    Joueur* titulaire_{nullptr};  // joueur qui détient la carte (non propriétaire)
    bool    active_{false};       // true = carte utilisable
};

#endif // CARTEDOUBLEGAIN_H
