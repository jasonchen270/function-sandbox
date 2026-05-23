// Render override "fancy_receipt" for key "default" (recency 1700000003000).
// Replaces the built-in one-line receipt. Uses the SAME generic Registry as the
// adjustments, just a different signature and the single-winner usage mode.
#include <cstdio>
#include <string>

#include "../core/receipt.h"

namespace {

std::string fancy_receipt(const Cart& cart, double total) {
    char buf[192];
    std::snprintf(buf, sizeof buf,
                  "=== RECEIPT ===\n"
                  "  subtotal: $%.2f\n"
                  "  total:    $%.2f\n"
                  "===============",
                  cart.subtotal, total);
    return buf;
}

const bool _registered = receipt::renderer().set("default", 1700000003000LL, fancy_receipt);

} // namespace
