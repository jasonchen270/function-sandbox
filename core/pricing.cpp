#include "pricing.h"

#include <string>

namespace pricing {

Registry<AdjustFn>& adjustments() {
    static Registry<AdjustFn> r;
    return r;
}

double base_price(const Cart& c) {
    return c.subtotal;
}

double price(const Cart& c) {
    double total = base_price(c);
    adjustments().for_each([&](const std::string&, const AdjustFn& fn) {
        total = fn(total, c);  // apply each adjustment's winner, in key order
    });
    return total;
}

} // namespace pricing
