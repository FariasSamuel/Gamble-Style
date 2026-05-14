/**
 * @file Joueur.cpp
 * @brief Implémentation de la classe Joueur.
 */
#include "Joueur.hpp"
#include "Case.hpp"
#include "Carte.hpp"
#include "CasePropriete.hpp"
#include <cstdlib>
#include <algorithm>

Joueur::Joueur(const std::string& nom, int capital)
    : nom(nom), capital(capital), val_propriete(0), position(0),
      dernier_lancer(0), compteur_double(0), compteur_tours(0),
      nb_proprietes_mock(0), use_mock_nb(false), case_actuelle(nullptr) {}

// Accesseurs
const std::string& Joueur::getNom()        const { return nom; }
int Joueur::getCapital()                   const { return capital; }
int Joueur::getValPropriete()              const { return val_propriete; }
int Joueur::getNbProprietes()              const { return use_mock_nb ? nb_proprietes_mock : proprietes.size(); }
Case* Joueur::getCaseActuelle()            const { return case_actuelle; }
int Joueur::getDernierLancer()             const { return dernier_lancer; }
int Joueur::getCompteurTours()             const { return compteur_tours; }
int Joueur::getNbCartes()                  const { return cartes.size(); }
int Joueur::getPosition() const {
    // Remplace 'position' par le nom exact de ta variable membre 
    // (celle utilisée dans setPosition)
    return position; 
}
const std::vector<CasePropriete*>& Joueur::getProprietes() const { return proprietes; }

// Modificateurs
void Joueur::setCapital(int c)           { capital = c; }
void Joueur::setDernierLancer(int v)     { dernier_lancer = v; }
void Joueur::resetCompteurTours()        { compteur_tours = 0; }

void Joueur::donnerCapital(int m)        { capital += m; }
void Joueur::enleverCapital(int m)       { capital -= m; }

Condition Joueur::conditionfinanciere() const {
    if (capital >= 0) return Condition::RICHE;
    if (val_propriete > 0) return Condition::BANQUEROUTE; // A encore des biens
    return Condition::FAILLITE; // Capital < 0 et plus aucun bien
}

int Joueur::capitalSolvabilite() const { return capital + val_propriete; }

void Joueur::misebanqueroute() {
    clearProprietes();
    capital = -1; // Force l'état de faillite
}

void Joueur::acheter(CasePropriete* c) {
    if (c && capital >= c->getPrix()) { 
        enleverCapital(c->getPrix());
        proprietes.push_back(c);
        c->setProprietaire(this);
        
        // ✨ LA LIGNE MAGIQUE À RAJOUTER :
        c->setAchetable(false); // La case n'est officiellement plus à vendre !
        
        val_propriete += c->getPrix(); 
    }
}
void Joueur::hypotequer(CasePropriete* cp) {
    auto it = std::find(proprietes.begin(), proprietes.end(), cp);
    if (it == proprietes.end()) return;
    
    donnerCapital(cp->getPrixHypotheque());
    val_propriete -= cp->getPrixHypotheque();
    
    // On ne supprime pas la propriété de la liste du joueur, 
    // et on ne la rend surtout pas achetable par les autres !
}

void Joueur::clearProprietes() {
    for (auto* p : proprietes) { p->setProprietaire(nullptr); p->setAchetable(true); }
    proprietes.clear(); val_propriete = 0; use_mock_nb = false;
}

void Joueur::setNbProprietesMock(int n) { nb_proprietes_mock = n; use_mock_nb = true; }

void Joueur::donnerCarte(Carte* c) { if (c && !possedeCarte(c->getId())) cartes.push_back(c); }
bool Joueur::possedeCarte(int id) const { for (auto* c : cartes) if (c->getId() == id) return true; return false; }
void Joueur::retirerCarte(int id) {
    cartes.erase(std::remove_if(cartes.begin(), cartes.end(), [id](Carte* c){ return c->getId() == id; }), cartes.end());
}

int Joueur::lancerde() { return (std::rand() % 6) + 1; }
void Joueur::setPosition(int idx) { position = idx; }
void Joueur::bougerjoueur(Case* dest) { if (dest) { case_actuelle = dest; position = dest->getIndex(); dest->setJoueurActif(this); } }
void Joueur::tour() {compteur_tours++; /* Logique de tour à implémenter selon IHM */ }
