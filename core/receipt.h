#pragma once
#include <functional>
#include <string>

#include "pricing.h"
#include "registry.h"

namespace receipt {

// Extension point #2: a SINGLE renderer. Unlike adjustments, only one wins: the
// newest override of key "default". If none is registered, a built-in is used.
// Same generic Registry, different signature and a different usage mode.
using RenderFn = std::function<std::string(const Cart&, double total)>;
Registry<RenderFn>& renderer();

std::string render(const Cart& c, double total);

} // namespace receipt
