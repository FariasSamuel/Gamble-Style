#include "Case.hpp"

Case::Case(int num) : num_case(num) {}

Case::~Case() = default;

int Case::get_num_case() const { return num_case; }

void Case::action() { /* no-op base */ }
