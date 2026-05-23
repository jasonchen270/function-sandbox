#include "receipt.h"

#include <cstdio>

namespace receipt {

Registry<RenderFn>& renderer() {
    static Registry<RenderFn> r;
    return r;
}

std::string render(const Cart& c, double total) {
    if (const RenderFn* fn = renderer().get("default")) {
        return (*fn)(c, total);  // the newest override wins
    }
    // built-in default, used when no override is registered
    char buf[128];
    std::snprintf(buf, sizeof buf, "subtotal %.2f | total %.2f", c.subtotal, total);
    return buf;
}

} // namespace receipt
