/**
 * @file CaseDepart.hpp
 * @brief Classe CaseDepart.
 * @project GambleStyle — GM4 INSA Rouen Normandie
 */
#ifndef CASEDEPART_HPP
#define CASEDEPART_HPP

#include "Case.hpp"

// ═══════════════════════════════════════════════════════════════
// CaseDepart
// ═══════════════════════════════════════════════════════════════
/**
 * @class CaseDepart
 * @brief Crédite un montant fixe à chaque passage.
 */
class CaseDepart : public Case {
public:
    explicit CaseDepart(int montant);
    void action() override;
    int getMontant() const;
private:
    int montant;
};

#endif // CASEDEPART_HPP
