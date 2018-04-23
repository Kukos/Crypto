#ifndef CRYPTOSYSTEM_H
#define CRYPTOSYSTEM_H

/*
    Merkle Helmann crypto system based on knapsack problem

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL3.0
*/

#include <gmp.h>
#include <stddef.h>

typedef struct MHCrypto_system
{
    size_t ____len;

    /* arrays with len = bits */
    mpz_t *____super_inc_seq;
    mpz_t *____public_key;

    mpz_t ____mod;
    mpz_t ____mult;

    gmp_randstate_t ____rstate;

} MHCrypto_system;

/*
    Create Merkle Helmann Crypto system

    PARAMS
    @IN len - public key len

    RETURN
    NULL iff failure
    Pointer to new MH Crypto system iff success
*/
MHCrypto_system *mh_cryptosystem_create(size_t len);

/*
    Destroy Merkle Helmann Crypto system

    PARAMS
    @IN mhc - pointer to MHC system

    RETURN
    This is a void function
*/
void mh_cryptosystem_destroy(MHCrypto_system *mhc);

/*
    Encrypt str to get cipher

    PARAMS
    @IN mhc - MHC system
    @IN str - string
    @OUT cipher - cipher from str

    RETURN
    This is a void function
*/
void mh_cryptosystem_encrypt(const MHCrypto_system *mhc, const char *str, mpz_t cipher);

/*
    Decrypt cipher into msg

    PARAMS
    @IN mhc - crypto system
    @IN cipher - cipher
    @OUT str -decoded str

    RETURN
    This is a void function
*/
void mh_cryptosystem_decrypt(const MHCrypto_system *mhc, const mpz_t cipher, char **str);

#endif