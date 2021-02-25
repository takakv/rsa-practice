#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

#define ARITHMETIC_MAX 0xFFFFFFFF

#if ULONG_MAX >= 0xFFFFFFFFFFFFFFFFULL
typedef unsigned long uint64;
#else
typedef unsigned long long uint64;
#endif

#if LONG_MAX >= 0x7FFFFFFFFFFFFFFFLL
typedef long int64;
# else
typedef long long int64;
#endif

// Fast modular exponentiation algorithm.
// NB! The current implementation requires the base to be at most
// a 32bit unsigned integer. This condition is not checked.
uint64 mod_pow(uint64 base, uint64 exponent, uint64 modulus);

// Gets the GCD of two numbers.
uint64 gcd(uint64, uint64);

// Gets the LCM of two numbers.
// NB! Current implementation requires that the product of both
// numbers fits inside a 64bit unsigned integer.
uint64 lcm(uint64, uint64);

// Non iterative implementation of the Extended Euclidean Algorithm.
// x * a + y * b = d
void eea(int64 a, int64 b, int64 *x, int64 *y, int64 *d);

// Carmichael's totient function with the special case
// of the factors of the integer being 2 known primes.
uint64 pxp_lambda(uint64 prime1, uint64 prime2);

// Returns true if n is (probably) prime, false if not.
// The accuracy depends on the internal primality testing method.
bool check_prime(uint64);

// Deterministic Miller-Rabin test for n < 2^64,
// proven by Feitsma's and Galway's exhaustive search.
// Returns true if n is prime, false if not.
bool d_miller_test(uint64);

// Submodule of the Miller-Rabin test for checking composite numbers.
bool check_composite(uint64 base, uint64 factor, uint64 target, int power);

// Get the encryption exponent.
uint64 get_ee(uint64);

int main() {
    uint64 prime1, prime2;

    // The current implementation requires both inputs to be at most 65535.
    // 65535 is the largest unsigned 16bit integer.
    prime1 = 101;
    prime2 = 89;

    bool is_prime = check_prime(prime1);
    if (is_prime) is_prime = check_prime(prime2);
    if (!is_prime) {
        printf("One of the numbers is not prime!");
        return 1;
    }

    uint64 modulus = prime1 * prime2;
    uint64 totient = pxp_lambda(prime1, prime2);
    uint64 enc_exp = get_ee(totient);

    int64 dec_exp, trash1, trash2;
    eea(enc_exp, totient, &dec_exp, &trash1, &trash2);
    while (dec_exp < 0) dec_exp += totient;

    printf("The public key is: (%lu, %lu)\n", modulus, enc_exp);
    printf("The private key is: %li. Don't share it with anyone!", dec_exp);

    return 0;
}

bool check_prime(uint64 n) {
    return d_miller_test(n);
}

bool d_miller_test(uint64 n) {
    // WARNING! The current implementation works ONLY if the input number
    // is a 32bit unsigned integer. Else overflow will happen.
    // The largest unsigned 32bit prime is 4294967291.
    if (n > ARITHMETIC_MAX) {
        printf("Sorry, the integer you used is too large for the system to handle.\n");
        return false;
    }

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

// This implementation works only if x^2 fits within an
// unsigned 64bit integer or else it will overflow.
// This condition is assumed and not checked!
bool check_composite(uint64 a, uint64 d, uint64 n, int r) {
    uint64 x = mod_pow(a, d, n);
    if (x == 1 || x == n - 1) return false;
    for (int i = 0; i < r - 1; ++i) {
        x = (x * x) % n;
        if (x == n - 1) return false;
    }
    return true;
}

// This implementation functions only if the value to be exponentiated
// and the modulus are guaranteed to fit within an unsigned 64bit integer.
// This condition is assumed and not checked!
uint64 mod_pow(uint64 base, uint64 exponent, uint64 modulus) {
    if (modulus == 1) return 0;
    uint64 result = (exponent & 1) ? base : 1;

    while (exponent != 0) {
        exponent >>= 1;
        base = (base * base) % modulus;
        if (exponent & 1) result = (result * base) % modulus;
    }
    return result;
}

// Non recursive implementation, as from experience,
// C programs frown upon recursive functions.
uint64 gcd(uint64 a, uint64 b) {
    while (true) {
        if (b == 0) return a;
        uint64 tmp = a;
        a = b;
        b = tmp % b;
    }
}

// This implementation works only if a * b fits within an
// unsigned 64bit integer or else it will overflow.
// This condition is assumed and not checked!
uint64 lcm(uint64 a, uint64 b) {
    return (a * b) / gcd(a, b);
}

// 2.107 Algorithm: Extended Euclidean algorithm
// "Handbook of Applied Cryptography" by Alfred J. Menezes et al page 67.
void eea(int64 a, int64 b, int64 *x, int64 *y, int64 *d) {
    if (b == 0) {
        *d = a; *x = 1; *y = 0;
        return;
    }
    int64 x2 = 1, x1 = 0, y2 = 0, y1 = 1;
    while (b > 0) {
        // NB! Floored division.
        int64 q = a / b, r = a - q * b;
        *x = x2 - q * x1, *y = y2 - q * y1;
        a = b; b = r;
        x2 = x1, x1 = *x, y2 = y1, y1 = *y;
    }
    *d = a, *x = x2, *y = y2;
}

uint64 pxp_lambda(uint64 prime1, uint64 prime2) {
    // Using Fermat's little theorem we see that
    // if p is prime, p - 1 satisfies lambda(p).
    return lcm(prime1 - 1, prime2 - 1);
}

// This implementation doesn't generate e randomly and picks a fixed
// one (prime) depending on the totient size.
// This is not generally an issue, and in done often in practise.
uint64 get_ee(uint64 totient) {
    int possible_ees[] = {65537, 51239, 21803, 12517, 4691, 997, 101, 13, 5, 3};
    int ee_count = sizeof(possible_ees) / sizeof(int);
    for (int i = 0; i < ee_count; ++i) {
        if (totient > possible_ees[i]) return possible_ees[i];
    }
    return 3;
}