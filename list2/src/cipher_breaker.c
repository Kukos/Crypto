#include <cipher_breaker.h>
#include <log.h>
#include <search.h>
#include <common.h>
#include <stdbool.h>

CMP(char)

static const char * const alphabet = " !\"',-.:;?ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
static const size_t alphabet_size = 62;


/*
    Check if char is correct

    PARAMS
    @IN c - char

    RETURN
    false iff not
    true iff yes
*/
static bool alpha_is_correct(char c);

/*
    Xor all ciphertext with text, write to another array and return it


    PARAMS
    @IN text - text to xor with another ciphers
    @IN ciphertext - cipher seq
    @IN len - len(siphertext)

    RETURN
    NULL iff failure
    Pointer to Darray * iff success
*/
static Darray **cipher_xor(const Darray *text, const Darray* const *ciphertext, size_t len);

/*
    Check alpha with all xors on posision num_alpha

    PARAMS
    @IN alpha - alpha from alphabet
    @IN num_alpha - index of alpha to check in xors
    @IN xor -array of xors
    @IN len - len(xor)

    RETURN
    false iff alpha is not correct
    true iff is correct
*/
bool check_alpha_in_xors(char alpha, size_t num_alpha, const Darray * const *xor, size_t len);

static bool alpha_is_correct(char c)
{
    return find_first_sorted((const void *)&c, alphabet, alphabet_size, cmp_char, sizeof(char)) != -1;
}

static Darray **cipher_xor(const Darray *text, const Darray* const *ciphertext, size_t len)
{
    Darray **xor;

    unsigned char c;
    size_t i;
    size_t j;
    size_t text_len = (size_t)darray_get_num_entries(text);

    TRACE();

    if (text == NULL)
        ERROR("text == NULL\n", NULL);

    xor = (Darray **)malloc(sizeof(Darray *) * len);
    if (xor == NULL)
        ERROR("Malloc error\n", NULL);

    for (i = 0; i < len; ++i)
    {
        xor[i] = darray_create(DARRAY_UNSORTED, 0, sizeof(unsigned char), NULL, NULL);
        for (j = 0; j < MIN(text_len, (size_t)darray_get_num_entries(ciphertext[i])); ++j)
        {
            c = ((unsigned char *)darray_get_array(text))[j] ^ ((unsigned char *)darray_get_array(ciphertext[i]))[j];
            (void)darray_insert(xor[i], (void *)&c);
        }
    }

    return xor;
}

bool check_alpha_in_xors(char alpha, size_t num_alpha, const Darray * const *xor, size_t len)
{
    size_t i;
    TRACE();

    for (i = 0; i < len; ++i)
        if (num_alpha < (size_t)darray_get_num_entries(xor[i]))
            if (!alpha_is_correct((char)((unsigned char)alpha ^ ((unsigned char *)darray_get_array(xor[i]))[num_alpha])))
                return false;

    return true;
}


void cipher_breaker_decode(const Darray *const *ciphertext_seq, size_t len)
{
    const Darray **xor;
    size_t i;
    size_t j;
    bool is_known = false;

    TRACE();

    if (ciphertext_seq == NULL)
        return;

    xor = (const Darray **)cipher_xor(ciphertext_seq[0], &ciphertext_seq[1], len - 1);
    for (i = 0; i < (size_t)darray_get_num_entries(ciphertext_seq[0]); ++i)
    {
        is_known = false;
        for (j = 0; j < alphabet_size; ++j)
            if (check_alpha_in_xors(alphabet[j], i, xor, len - 1))
            {
                printf("%c ", alphabet[j]);
                is_known = true;
            }
        if (!is_known)
            printf("unknown");

        printf("\n");
    }

    for (i = 0; i < len - 1; ++i)
        darray_destroy((Darray *)xor[i]);

    FREE(xor);
}