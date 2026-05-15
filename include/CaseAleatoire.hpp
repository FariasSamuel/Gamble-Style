// CaseAleatoire.hpp
// Case "pioche" contenant un deck de cartes en rotation FIFO.
// À chaque atterrissage, la première carte est piochée, son titulaire est
// défini via setTitulaire(), puis son effet est appliqué via action().
// La carte retourne ensuite en fin de file pour la prochaine rotation.

#ifndef CASEALEATOIRE_H
#define CASEALEATOIRE_H

#include "Case.hpp"
#include <deque>

class Carte;
class Joueur;

// CaseAleatoire — case tirage de carte avec deck tournant.
// Invariant : la file n'est jamais vide si au moins une carte a été ajoutée.
class CaseAleatoire : public Case {
public:
    CaseAleatoire();
    explicit CaseAleatoire(int num);
    ~CaseAleatoire();

    // Ajoute une carte à la fin du deck (la case prend la propriété de l'objet).
    void ajouterCarte(Carte* c);

    // Retourne un pointeur sur la prochaine carte à piocher sans la consommer.
    // Utilisé par PlateauSFML pour préparer l'overlay avant d'appeler action().
    Carte* getPremiereCarteDeFile();

    // Pioche la première carte, lui assigne le joueur actif, applique son effet,
    // puis la remet en fin de file.
    void action() override;

    // Surcharge pour propager le joueur actif avant l'appel à action().
    void setJoueurActif(Joueur* j);

private:
    std::deque<Carte*> file;  // deck en rotation (propriétaire des objets Carte)
};

#endif // CASEALEATOIRE_H
