/**
 * @file CarteDeplacement.cpp
 * @brief Implémentation de CarteDeplacement.
 */
#include "CarteDeplacement.hpp"
#include "Joueur.hpp"
#include "Plateau.hpp"
#include "Case.hpp"
#include <memory>
#include <vector>

CarteDeplacement::CarteDeplacement(Joueur* joueur, Plateau* plateau, int caseIndex)
    : Carte("Deplacement", "Deplacez-vous vers la case " + std::to_string(caseIndex) + "."),
      joueur(joueur), plateau(plateau), caseIndex(caseIndex) {}

void CarteDeplacement::action() {
    if (!joueur) return;
    Case* dest = (plateau) ? plateau->getCase(caseIndex) : nullptr;
    
    if (!dest) {
        joueur->setPosition(caseIndex);
        struct CaseMock : public Case {
            CaseMock(int idx) : Case(idx) {}
            void action() override {}
        };
        static std::vector<std::unique_ptr<CaseMock>> mocks;
        for (auto& m : mocks) {
            if (m->getIndex() == caseIndex) { 
                joueur->bougerjoueur(m.get()); 
                return; 
            }
        }
        mocks.push_back(std::unique_ptr<CaseMock>(new CaseMock(caseIndex)));
        joueur->bougerjoueur(mocks.back().get());
        return;
    }
    joueur->bougerjoueur(dest);
}
