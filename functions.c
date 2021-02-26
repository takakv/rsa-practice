#include "functions.h"

bool check_prime(uint64 n) {
    return d_miller_test(n);
}

bool d_miller_test(uint64 n) {
    if (n < 2) return false;
    // (not strictly needed for the deterministic variant)
    if (n == 2) return true;
    if ((n & 1) == 0) return false;

    // Write n as (2^r * d) + 1 with d odd.
    // This can be done by factoring out powers of 2 from n - 1;
    int r = 0;
    uint64 d = n - 1;

    while ((d & 1) == 0) {
        d >>= 1;
        ++r;
    }

    // Check against the first 12 primes, hence "deterministic".
    int first_12_primes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
    for (int i = 0; i < 12; ++i) {
        if (first_12_primes[i] == n) return true;
        if (check_composite(first_12_primes[i], d, n, r)) return false;
    }
    return true;
}

bool check_composite(uint64 a, uint64 d, uint64 n, int r) {
    uint128 x = mod_pow(a, d, n);
    if (x == 1 || x == n - 1) return false;
    for (int i = 0; i < r - 1; ++i) {
        x = (x * x) % n;
        if (x == n - 1) return false;
    }
    return true;
}

uint64 mod_pow(const uint64 base, uint128 exponent, uint128 modulus) {
    if (modulus == 1) return 0;
    uint128 tmp_base = base;
    uint128 result = (exponent & 1) ? tmp_base : 1;

    while (exponent != 0) {
        exponent >>= 1;
        tmp_base = (tmp_base * tmp_base) % modulus;
        if (exponent & 1) result = (result * tmp_base) % modulus;
    }
    return (uint64) result;
}

// Non recursive implementation, as from experience,
// C programmers frown upon recursive functions.
uint64 gcd(uint64 a, uint64 b) {
    while (b != 0) {
        uint64 tmp = a;
        a = b;
        b = tmp % b;
    }
    return a;
}

uint128 lcm(uint64 a, uint64 b) {
    uint128 tmp_a = a, tmp_b = b;
    return (tmp_a * tmp_b) / gcd(a, b);
}

// 2.107 Algorithm: Extended Euclidean algorithm
// "Handbook of Applied Cryptography" by Alfred J. Menezes et al page 67.
// The current implementation is potentially unsafe!
void eea(int128 a, int128 b, int128 *x, int128 *y, int128 *d) {
    if (b == 0) {
        *d = a, *x = 1, *y = 0;
        return;
    }
    int128 x2 = 1, x1 = 0, y2 = 0, y1 = 1;
    while (b > 0) {
        // NB! Floored division.
        int128 q = a / b, r = a - q * b;
        *x = x2 - q * x1, *y = y2 - q * y1;
        a = b, b = r;
        x2 = x1, x1 = *x, y2 = y1, y1 = *y;
    }
    *d = a, *x = x2, *y = y2;
}

uint128 pxp_lambda(uint64 prime1, uint64 prime2) {
    // Using Fermat's little theorem we see that
    // if p is prime, p - 1 satisfies lambda(p).
    return lcm(prime1 - 1, prime2 - 1);
}

// This implementation doesn't generate e randomly and picks a fixed
// one (prime) depending on the totient size.
// This is not generally an issue, and is done often in practise.
uint64 get_ee(uint128 totient) {
    int possible_ees[] = {65537, 51239, 21803, 12517, 4691, 997, 101, 13, 5, 3};
    int ee_count = sizeof(possible_ees) / sizeof(int);
    for (int i = 0; i < ee_count; ++i) {
        if (totient > possible_ees[i]) return possible_ees[i];
    }
    return 3;
}
