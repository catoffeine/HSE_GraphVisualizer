#pragma once

#include <cstdint>
#include <cmath>


bool IsPrime(uint16_t x) {
    if (x < 2) return false;
    for (uint16_t i = 2; i * i <= x; ++i)
        if (x % i == 0) return false;
    return true;
}

bool IsPrimePower(uint16_t q, uint16_t &p, uint16_t &k) {
    for (uint16_t base = 2; base <= std::sqrt(q); ++base) {
        uint16_t power = base;
        uint16_t exponent = 1;
        while (power < q) {
            power *= base;
            exponent++;
            if (power == q && IsPrime(base)) {
                p = base;
                k = exponent;
                return true;
            }
        }
    }
    if (IsPrime(q)) {
        p = q;
        k = 1;
        return true;
    }
    return false;
}