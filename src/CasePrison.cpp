// CasePrison.cpp
// Implémentation de la case "Station Prison".
// Un joueur emprisonné doit lancer un double pour sortir.
// Après 3 tours sans double, il est libéré automatiquement.
// Les résultats peuvent être forcés pour les tests unitaires (has_forced_ persistant).

#include "CasePrison.hpp"
#include "Joueur.hpp"
#include "CarteSortiePrison.hpp"

CasePrison::CasePrison(int num)
    : Case(num), utiliserCarteReponse_(false), forced_d1_(0), forced_d2_(0), has_forced_(false)
{
}

CasePrison::~CasePrison() = default;

// Active ou désactive la simulation d'utilisation de carte (pour les tests).
void CasePrison::setUtiliserCarteReponse(bool val) { utiliserCarteReponse_ = val; }

// Fixe un résultat de dés déterministe. Persistant : tous les tours suivants
// utilisent ce résultat jusqu'à un nouvel appel ou une réinitialisation.
void CasePrison::forcerResultatDes(int d1, int d2)
{
    forced_d1_  = d1;
    forced_d2_  = d2;
    has_forced_ = true;
}

// Retourne le nombre de tours passés en prison par j (0 si non emprisonné).
int CasePrison::getTourPrison(Joueur* j)
{
    auto it = tours_.find(j);
    if (it == tours_.end()) return 0;
    return it->second;
}

// Résolution d'un tour en prison pour le joueur actif :
//   - carte Sortie Prison + utiliserCarteReponse_ → libération immédiate
//   - double → libération
//   - pas de double → compteur++, libération automatique à 3 tours
void CasePrison::action()
{
    Joueur* j = getJoueurActif();
    if (!j) return;

    // Utilisation de la carte Sortie de Prison (décision simulée pour les tests)
    if (j->possedeCarte(CarteSortiePrison::ID) && utiliserCarteReponse_) {
        tours_[j] = 0;  // libéré sans lancer les dés
        return;
    }

    // Lancer les dés (forcé ou aléatoire)
    int d1, d2;
    if (has_forced_) {
        d1 = forced_d1_;
        d2 = forced_d2_;
        // Le résultat forcé est persistant : a été lancé plusieurs fois de suite en test
    } else {
        d1 = j->lancerde();
        d2 = j->lancerde();
    }

    if (d1 == d2) {
        // Double : le joueur est libéré
        tours_[j] = 0;
        return;
    }

    // Pas de double : incrément du compteur
    tours_[j] = getTourPrison(j) + 1;
    // Libération automatique après 3 tours sans double
    if (tours_[j] >= 3) {
        tours_[j] = 0;
    }
}
