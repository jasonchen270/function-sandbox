// Adjustment override "tax_v1" for key "100-tax" (recency 1700000000000).
// SUPERSEDED example: tax_v2 has a newer recency, so this one loses. Kept here
// to demonstrate the newest-wins policy. core/ is never edited.
#include "../core/pricing.h"

namespace {

double tax_v1(double total, const Cart& cart) {
    return total * 1.08;  // 8% tax (older)
}

const bool _registered = pricing::adjustments().set("100-tax", 1700000000000LL, tax_v1);

} // namespace
