#include <base64.h>
#include <log.h>
#include <string.h>

#define PADDING_CHAR '='

static const BYTE base64_revtable[] =
{
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
	64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
	64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64
};

static const BYTE base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

BYTE *base64_encode(const BYTE *bytes, size_t len, size_t *size)
{
    size_t i;
    DWORD acc = 0;
    size_t bits = 0;
    BYTE *ret;
    size_t ret_size;
    size_t ret_index = 0;

    TRACE();

    if (bytes == NULL)
        ERROR("bytes == NULL\n", NULL);

    ret_size = ((len + 2) / 3) * 4;

    ret = (BYTE *)malloc(ret_size + 1);
    if (ret == NULL)
        ERROR("malloc error\n", NULL);

    (void)memset(ret, PADDING_CHAR, ret_size);
    ret[ret_index] = '\0';

    for (i = 0; i < len; ++i)
    {
        acc = acc << 8 | bytes[i];
        bits += 8;
        while (bits >= 6)
        {
            bits -= 6;
            ret[ret_index] = base64_table[(acc >> bits) & 0x3f];
            ++ret_index;
        }
    }

    if (bits > 0)
    {
        acc <<= 6 - bits;
        ret[ret_index] = base64_table[acc & 0x3f];
        ++ret_index;
    }

    if (size != NULL)
        *size = ret_size;

    ret[ret_index] = '\0';
    return ret;
}

BYTE *base64_decode(const BYTE *bytes, size_t len, size_t *size)
{
    BYTE *ret;
    size_t ret_size;
    size_t i;
    DWORD acc = 0;
    size_t bits = 0;
    size_t ret_index = 0;

    TRACE();

    if (bytes == NULL)
        ERROR("bytes == NULL\n", NULL);

    ret_size = len;

    ret = (BYTE *)malloc(ret_size + 1);
    if (ret == NULL)
        ERROR("malloc error\n", NULL);

    ret[ret_index] = '\0';

    for (i = 0; i < len; ++i)
    {
        if (bytes[i] == PADDING_CHAR)
            continue;

        acc = (acc << 6) | base64_revtable[bytes[i]];
        bits += 6;
        if (bits >= 8)
        {
            bits -= 8;
            ret[ret_index] = (BYTE)(acc >> bits);
            ++ret_index;
        }
    }

    if (size != NULL)
        *size = ret_index;

    ret[ret_index] = '\0';

    return ret;
}