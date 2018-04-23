#include <common.h>
#include <test.h>
#include <cryptosystem.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define N 1000

test_f test_crypto_system(size_t size)
{
    MHCrypto_system *mhc;
    size_t len = size / 8 - 1;
    char str [len];
    char *ptr;
    mpz_t cipher;
    size_t i;
    size_t j;

    mpz_init(cipher);

    mhc = mh_cryptosystem_create(size);

    for (i = 0; i < N; ++i)
    {
        for (j = 0; j < len; ++j)
            str[j] = rand() % 26 + 'a';

        mh_cryptosystem_encrypt(mhc, str, cipher);
        mh_cryptosystem_decrypt(mhc, cipher, &ptr);

        T_EXPECT(strcmp(str, ptr), 0);
    }


    mh_cryptosystem_destroy(mhc);
}


void test(void)
{
    TEST(test_crypto_system(64));
    TEST(test_crypto_system(128));
    TEST(test_crypto_system(1024));
}

int main(void)
{
    TEST_INIT("MH Cryptosystem");
    test();

    TEST_SUMMARY();
    return 0;
}