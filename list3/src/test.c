#include <test.h>
#include <aes.h>
#include <common.h>

static const BYTE key[] = "Kukos C Master";
#define MAX_BITS 5

static void rand_bytes(char *t, size_t size)
{
    size_t i;
    for (i = 0; i < size; ++i)
        t[i] = (rand() % 26) + 'a';

    t[size] = '\0';
}

test_f test_aes_cbc(void)
{
    size_t size;
    size_t rsize;
    char t[BIT(MAX_BITS) + 1];
    BYTE *c;
    BYTE *p;

    for (size = BIT(0); size < BIT(MAX_BITS); ++size)
    {
        rand_bytes(t, size);
        rsize = aes_encrypt_cbc((BYTE *)t, key, NULL, &c);
        T_CHECK(rsize != -1);
        T_CHECK(aes_decrypt_cbc(c, rsize, key, &p) != -1);
        T_EXPECT(strcmp((char *)t, (char *)p), 0);

        FREE(c);
        FREE(p);
    }
}

test_f test_aes_ctr(void)
{
    size_t size;
    size_t rsize;
    char t[BIT(MAX_BITS) + 1];
    BYTE *c;
    BYTE *p;

    for (size = BIT(0); size < BIT(MAX_BITS); ++size)
    {
        rand_bytes(t, size);
        rsize = aes_encrypt_ctr((BYTE *)t, key, NULL, &c);
        T_CHECK(rsize != -1);
        T_CHECK(aes_decrypt_ctr(c, rsize, key, &p) != -1);
        T_EXPECT(strcmp((char *)t, (char *)p), 0);

        FREE(c);
        FREE(p);
    }
}

test_f test_aes_ofb(void)
{
    size_t size;
    size_t rsize;
    char t[BIT(MAX_BITS) + 1];
    BYTE *c;
    BYTE *p;

    for (size = BIT(0); size < BIT(MAX_BITS); ++size)
    {
        rand_bytes(t, size);
        rsize = aes_encrypt_ofb((BYTE *)t, key, NULL, &c);
        T_CHECK(rsize != -1);
        T_CHECK(aes_decrypt_ofb(c, rsize, key, &p) != -1);
        T_EXPECT(strcmp((char *)t, (char *)p), 0);

        FREE(c);
        FREE(p);
    }
}

void test(void)
{
    TEST(test_aes_cbc());
    TEST(test_aes_ctr());
    TEST(test_aes_ofb());
}

int main(void)
{
    TEST_INIT("AES");
    test();
    TEST_SUMMARY();
}