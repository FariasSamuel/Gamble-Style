#include "CasePropriete.hpp"

CasePropriete::CasePropriete(int num)
    : Case(num), prix_achat(0), prix_hypotheque(0), achetable(true)
{
}

CasePropriete::~CasePropriete() = default;

void CasePropriete::action() {
    // to implement
}
