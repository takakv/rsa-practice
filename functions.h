#include <stdbool.h>
#include <limits.h>

#define UINT64_MAX 0xFFFFFFFFFFFFFFFFULL
#define INT64_MAX 0x7FFFFFFFFFFFFFFFLL

typedef unsigned __int128 uint128;
typedef __int128 int128;
typedef unsigned long long ull;

#if ULONG_MAX >= UINT64_MAX
typedef unsigned long uint64;
#else
typedef unsigned long long uint64;
#endif

#if LONG_MAX >= INT64_MAX
typedef long int64;
# else
typedef long long int64;
#endif

// Fast modular exponentiation algorithm.
uint64 mod_pow(uint64 base, uint128 exponent, uint128 modulus);

// Gets the GCD of two numbers.
uint64 gcd(uint64, uint64);

// Gets the LCM of two numbers.
uint128 lcm(uint64, uint64);

// Non iterative implementation of the Extended Euclidean Algorithm.
// x * a + y * b = d
// The current implementation is potentially unsafe!
void eea(int128 a, int128 b, int128 *x, int128 *y, int128 *d);

// Carmichael's totient function with the special case
// of the factors of the integer being 2 known primes.
uint128 pxp_lambda(uint64 prime1, uint64 prime2);

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
uint64 get_ee(uint128);
