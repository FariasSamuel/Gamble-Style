/**
 * @file CaseEvenement.cpp
 * @brief Implémentation de CaseEvenement.
 * @project GambleStyle — GM4 INSA Rouen Normandie
 */
#include "CaseEvenement.hpp"
#include "Joueur.hpp"
#include "MinijeuTron.hpp" // Intégration du polymorphisme avec Tron
#include <cstdlib>

CaseEvenement::CaseEvenement()
    : Case("Evenement", -1),
      mise(0), gagnant(nullptr), minijeu_joue(false)
{
    // On initialise la liste avec le mini-jeu Tron au lieu du jeu aléatoire par défaut
    minijeux.push_back(new MinijeuTron());
}

void CaseEvenement::ajouterJoueur(Joueur* j)  { if (j) joueurs.push_back(j); }
void CaseEvenement::setMise(int m)            { mise = m; }
void CaseEvenement::setGagnant(Joueur* j)     { gagnant = j; }
void CaseEvenement::clearMinijeux()           { minijeux.clear(); }
bool CaseEvenement::minijeuJoue()       const { return minijeu_joue; }

void CaseEvenement::setClassement(const std::vector<Joueur*>& cl) {
    classement = cl;
}

Minijeu* CaseEvenement::choixminijeu() {
    if (minijeux.empty()) return nullptr;
    int idx = std::rand() % static_cast<int>(minijeux.size());
    return minijeux[idx];
}

void CaseEvenement::distribution() {
    std::vector<Joueur*> ordre = classement.empty() ? joueurs : classement;
    if (ordre.empty()) return;

    int pot = mise * static_cast<int>(joueurs.size());

    // Prélèvement de la mise
    for (auto* j : joueurs) j->enleverCapital(mise);

    // Récompense : 75% au 1er, 25% au 2e
    if (ordre.size() >= 1) ordre[0]->donnerCapital(pot * 3 / 4);
    if (ordre.size() >= 2) ordre[1]->donnerCapital(pot / 4);
}

void CaseEvenement::action() {
    Minijeu* jeu = choixminijeu();
    if (!jeu || joueurs.size() < 2) {
        minijeu_joue = true;
        return;
    }

    // Gestion du duel ou du jeu à plusieurs
    if (joueurs.size() == 2) {
        Joueur* g = jeu->jouer(joueurs[0], joueurs[1]);
        gagnant = g;
        classement = { g, (g == joueurs[0] ? joueurs[1] : joueurs[0]) };
    } else {
        // Pour 3 ou 4 joueurs
        Joueur** r = (joueurs.size() == 3) 
            ? jeu->jouer(joueurs[0], joueurs[1], joueurs[2])
            : jeu->jouer(joueurs[0], joueurs[1], joueurs[2], joueurs[3]);
        
        classement.clear();
        for (size_t i = 0; i < joueurs.size(); ++i) {
            classement.push_back(r[i]);
        }
        gagnant = r[0];
        delete[] r; // Libération de la mémoire du tableau de pointeurs
    }

    distribution();
    minijeu_joue = true;
}
