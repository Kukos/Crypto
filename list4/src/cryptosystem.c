#include <cryptosystem.h>
#include <log.h>
#include <gmp.h>
#include <stdlib.h>
#include <common.h>
#include <time.h>
#include <string.h>
#include <bitset.h>
#include <cstring.h>

/*
    Helpers to allocate memory for mhc field and create it

    PARAMS
    @IN mhc - pointer to MHC system

    RETURN
    Those are void functions
*/
static void mh_cryptosystem_create_super_inc_seq(MHCrypto_system *mhc);
static void mh_cryptosystem_create_mod(MHCrypto_system *mhc);
static void mh_cryptosystem_create_mult(MHCrypto_system *mhc);
static void mh_cryptosystem_create_public_key(MHCrypto_system *mhc);

/* numbers between [(2^(i-1) - 1) * 2^100; 2^(i-1) * 2^100] */
static void mh_cryptosystem_create_super_inc_seq(MHCrypto_system *mhc)
{
    mpz_t r;
    mpz_t pown;
    mpz_t k;

    size_t i;

    TRACE();

    mpz_init(pown); /* 2^n */
    mpz_init(r); /* rand */
    mpz_init(k);

    mhc->____super_inc_seq = (mpz_t *)malloc(sizeof(mpz_t) * mhc->____len);
    if (mhc->____super_inc_seq == NULL)
        return;

    mpz_ui_pow_ui(pown, 2, (unsigned int)mhc->____len);

    for (i = 1; i <= mhc->____len; ++i)
    {
        mpz_urandomb(r, mhc->____rstate, mhc->____len);

        /* steps below do: k = (2^(i - 1) - 1) * pown */

        /* k = (2^(i-1) */
        mpz_ui_pow_ui(k, 2, i - 1);

        /* k = (k - 1) * pown */
        mpz_sub_ui(k, k, 1);
        mpz_mul(k, k, pown);

        /* add k to random to create super inc seq: r = r + k */
        mpz_add(r, r, k);

        /* write r to super inc seq */
        mpz_init(mhc->____super_inc_seq[i - 1]);
        mpz_set(mhc->____super_inc_seq[i - 1], r);

    }

    mpz_clear(r);
    mpz_clear(pown);
    mpz_clear(k);
}

static void mh_cryptosystem_create_mod(MHCrypto_system *mhc)
{
    mpz_t r;
    mpz_t pown;

    TRACE();

    mpz_init(r);
    mpz_init(pown);

    /* big power of n 2 ^ (2n + 50) */
    mpz_ui_pow_ui(pown, 2, (unsigned long)(mhc->____len << 1) + 30);
    mpz_sub_ui(pown, pown, 1);

    /* random [0, 2^201 - 2] */
    mpz_urandomm(r, mhc->____rstate, pown);

    /* r = r^pown + 2 */
    mpz_add(r, r, pown);
    mpz_add_ui(r, r, 2);

    mpz_init(mhc->____mod);
    mpz_set(mhc->____mod, r);

    mpz_clear(r);
    mpz_clear(pown);
}

static void mh_cryptosystem_create_mult(MHCrypto_system *mhc)
{
    TRACE();

    mpz_t mult;
    mpz_t gcd;

    mpz_init(mult);
    mpz_init(gcd);
    mpz_init(mhc->____mult);

    /* mult = mod - 4 */
    mpz_sub_ui(mult, mhc->____mod, 4);

    /* mult = rand [2, Mod - 2] */
    mpz_urandomm(mult, mhc->____rstate, mult);
    mpz_add_ui(mult, mult, 2);

    /* gcd = gcd(rand[2, Mod - 2], mod) */
    mpz_gcd(gcd, mult, mhc->____mod);

    mpz_div(mhc->____mod, mhc->____mod, gcd);
    mpz_div(mhc->____mult, mult, gcd);

    mpz_clear(mult);
    mpz_clear(gcd);
}

static void mh_cryptosystem_create_public_key(MHCrypto_system *mhc)
{
    size_t i;

    TRACE();

    mhc->____public_key = (mpz_t *)malloc(sizeof(mpz_t) * mhc->____len);
    if (mhc->____public_key == NULL)
        return;

    for (i = 0; i < mhc->____len; ++i)
    {
        mpz_init(mhc->____public_key[i]);
        mpz_mul(mhc->____public_key[i], mhc->____super_inc_seq[i], mhc->____mult);
        mpz_mod(mhc->____public_key[i], mhc->____public_key[i], mhc->____mod);
    }
}

MHCrypto_system *mh_cryptosystem_create(size_t bits)
{
    MHCrypto_system *mhc;

    TRACE();

    mhc = (MHCrypto_system *)malloc(sizeof(MHCrypto_system));
    if (mhc == NULL)
        ERROR("malloc error\n", NULL);

    mhc->____len = bits;
    gmp_randinit_mt(mhc->____rstate);
    gmp_randseed_ui(mhc->____rstate, (unsigned long)time(NULL));

    /* create all variables */
    mh_cryptosystem_create_super_inc_seq(mhc);
    mh_cryptosystem_create_mod(mhc);
    mh_cryptosystem_create_mult(mhc);
    mh_cryptosystem_create_public_key(mhc);

    return mhc;
}

void mh_cryptosystem_destroy(MHCrypto_system *mhc)
{
    size_t i;

    TRACE();

    if (mhc == NULL)
        return;

    gmp_randclear(mhc->____rstate);
    mpz_clear(mhc->____mod);
    mpz_clear(mhc->____mult);

    for (i = 0; i < mhc->____len; ++i)
    {
        mpz_clear(mhc->____super_inc_seq[i]);
        mpz_clear(mhc->____public_key[i]);
    }

    FREE(mhc->____public_key);
    FREE(mhc->____super_inc_seq);
    FREE(mhc);
}

void mh_cryptosystem_encrypt(const MHCrypto_system *mhc, const char *str, mpz_t cipher)
{
    mpz_t temp;

    size_t i;
    size_t j;
    size_t len;

    TRACE();

    if (mhc == NULL)
        return;

    if (str == NULL)
        return;

    mpz_init(temp);
    mpz_set_ui(cipher, 0);
    len = strlen(str);

    /*cipher = sum[str[i] * public_key[i]] */
    for (i = 0; i < len; ++i)
        for (j = 0; j < 8; ++j)
        {
            mpz_mul_ui(temp, mhc->____public_key[(i << 3) + j], GET_BIT((unsigned long long)str[i], j));
            mpz_add(cipher, cipher, temp);
        }
    mpz_clear(temp);
}

void mh_cryptosystem_decrypt(const MHCrypto_system *mhc, const mpz_t cipher, char **str)
{
    mpz_t inverse;
    mpz_t sol;

    Bitset *set;
    ssize_t i;
    size_t j;

    BYTE val;
    BYTE bit;
    BYTE *b;

    String *string;

    TRACE();

    string = string_create();

    set = bitset_create(mhc->____len + 1);
    if (set == NULL)
        return;

    mpz_init(inverse);
    mpz_init(sol);

    mpz_invert(inverse, mhc->____mult, mhc->____mod);
    mpz_mul(sol, cipher, inverse);
    mpz_mod(sol, sol, mhc->____mod);

    j = 0;
    for (i = (ssize_t)mhc->____len - 1; i >= 0; --i)
    {
        if (mpz_cmp(sol, mhc->____super_inc_seq[i]) >= 0 && mpz_cmp_ui(sol, 0) > 0)
        {
            mpz_sub(sol, sol, mhc->____super_inc_seq[i]);
            bitset_set_bit(set, j);
        }
        ++j;
    }

    i = 0;
    val = 0;
    for_each(set, Bitset, b, bit)
    {
        if (i == 8)
        {
            REVERSE_BITS(val);
            if (val != 0)
               string_insert_c(string, (char)val, 0);

            val = 0;
            i = 0;
        }
        SET_BIT_VALUE(val, 7 - i, bit);
        ++i;
    }

    *str = malloc(string_get_length(string) + 1);
    memcpy(*str, string_get_str(string), string_get_length(string));

    string_destroy(string);
    bitset_destroy(set);
    mpz_clear(inverse);
    mpz_clear(sol);
}