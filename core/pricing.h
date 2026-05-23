#pragma once
#include <functional>

#include "registry.h"

struct Cart {
    double subtotal;
};

namespace pricing {

// Extension point #1: adjustments STACK. Every key's winner is applied to the
// running total, in key order. Name keys with a numeric prefix (e.g. "100-tax")
// to control the sequence. Two overrides of the same key compete; newest wins.
using AdjustFn = std::function<double(double total, const Cart&)>;
Registry<AdjustFn>& adjustments();

double base_price(const Cart& c);
double price(const Cart& c);  // base_price, then every adjustment winner

} // namespace pricing
