// This program implements the RSA algorithm and supports unsigned
// integers up to 64 bits. To guarantee this, it must use the __int128
// type, which is not part of the C standard and is ONLY available for
// x64 systems and select compilers. Its range can be expanded by using
// and arbitrary precision arithmetic library such as GMP.

// NOTE! This program is unsafe in both the programmatic sense such as
// potential overflows or memory leaks and the likes, but also in the
// cryptographic sense: small numbers, timing attacks, ...

// I would love it if you'd let me know in case you fix/improve
// this program so that I can learn more as well! - github.com/takakv

#include <stdio.h>
#include "functions.h"

int main()
{
    uint64 prime1, prime2;

    prime1 = 101;
    prime2 = 89;

    //prime1 = 18446744073709551557U;
    //prime2 = 18446744073709551533U;

    bool is_prime = check_prime(prime1);
    if (is_prime) is_prime = check_prime(prime2);
    if (!is_prime)
    {
        printf("One of the numbers is not prime!");
        return 1;
    }

    uint128 modulus = (uint128) prime1 * prime2;
    uint128 totient = pxp_lambda(prime1, prime2);
    uint64 enc_exp = get_ee(totient);

    int128 dec_exp, trash1, trash2;
    eea(enc_exp, totient, &dec_exp, &trash1, &trash2);
    while (dec_exp < 0) dec_exp += totient;

    if (modulus <= UINT64_MAX)
    {
        printf("The public key is: (%lli, %lli).",
               (ull) modulus, (ull) enc_exp);
    }
    else
    {
        printf("The public key is too large to display ");
        printf("using standard functions. As a challenge, ");
        printf("try to write a function to print 128bit numbers.");
    }

    printf("\n");

    if (dec_exp <= INT64_MAX)
    {
        printf("The private key is: %lli. Don't share it with anyone!",
               (ull) dec_exp);
    }
    else
    {
        printf("The private key is too large to display ");
        printf("using standard functions. As a challenge, ");
        printf("try to write a function to print 128bit numbers.");
    }

    return 0;
}
