// Adjustment override "discount" for key "200-discount" (recency 1700000002000).
// Applies after tax because "200-..." sorts after "100-...". core/ untouched.
#include "../core/pricing.h"

namespace {

double discount(double total, const Cart& cart) {
    return total - 5.0;  // flat $5 off
}

const bool _registered = pricing::adjustments().set("200-discount", 1700000002000LL, discount);

} // namespace
