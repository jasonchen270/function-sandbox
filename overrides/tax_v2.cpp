// Adjustment override "tax_v2" for key "100-tax" (recency 1700000001000).
// WINNER: newer than tax_v1, same key, so this supersedes it. core/ untouched.
#include "../core/pricing.h"

namespace {

double tax_v2(double total, const Cart& cart) {
    return total * 1.05;  // 5% tax (newer)
}

const bool _registered = pricing::adjustments().set("100-tax", 1700000001000LL, tax_v2);

} // namespace
