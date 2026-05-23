#include <cstdio>

#include "core/pricing.h"
#include "core/receipt.h"

int main() {
    Cart c{200.0};
    double total = pricing::price(c);
    std::printf("%s\n", receipt::render(c, total).c_str());
    return 0;
}
