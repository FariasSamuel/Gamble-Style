#include "CasePrison.hpp"

CasePrison::CasePrison(int num)
    : Case(num)
{
    for (int i = 0; i < 4; ++i) liste_tour[i] = 0;
}

CasePrison::~CasePrison() = default;

void CasePrison::action() {
    // to implement
}
