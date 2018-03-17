#include <filebuffer.h>
#include <cipher_breaker.h>
#include <log.h>
#include <common.h>

const char* const path_to_ciphertext = "./cipher.txt";

___before_main___(1) void init(void);
___after_main___(1) void deinit(void);

___before_main___(1) void init(void)
{
    (void)log_init(stdout, NO_LOG_TO_FILE);
}

___after_main___(1) void deinit(void)
{
    log_deinit();
}

/*
    Read file and parse it to seq of ciphertext

    PARAMS
    @IN file - path to file
    @OUT size - size of returned array (iff not NULL)

    RETURN
    NULL iff failure
    Pointer to array of Darray * iff success
*/
Darray **get_ciphertext_seq_from_file(const char *file, size_t *size);

Darray **get_ciphertext_seq_from_file(const char *file, size_t *size)
{
    const int number_base = 2;

    File_buffer *fb;
    char *buf;
    Darray **ciphertext_seq;

    char temp[BIT(5)] = {0};
    unsigned char c;

    size_t i;
    size_t j;
    int reads;

    size_t num;

    TRACE();

    fb = file_buffer_create_from_path(file, PROT_READ | PROT_WRITE, O_RDWR);
    if (fb == NULL)
        ERROR("file buffer create error\n", NULL);

    buf = file_buffer_get_buff(fb);

    /* get number of text */
    if (sscanf(buf, "%zu%n", &num, &reads) == -1)
    {
        file_buffer_destroy(fb);
        ERROR("Sscanf error\n", NULL);
    }

    LOG("Num of CText = %zu\n", num);

    ciphertext_seq = (Darray **)malloc(sizeof(Darray *) * num);
    if (ciphertext_seq == NULL)
    {
        file_buffer_destroy(fb);
        ERROR("malloc error\n", NULL);
    }

    buf += reads + 1;
    i = (size_t)reads + 1;
    j = 0;
    while (i < (size_t)file_buffer_get_size(fb))
    {
        ciphertext_seq[j] = darray_create(DARRAY_UNSORTED, 0, sizeof(unsigned char), NULL, NULL);
        while (*buf != '\n')
        {
            /* get char in binary mode */
            if (sscanf(buf, "%s%n", temp, &reads) == -1)
                break;

            buf += reads;
            i += (size_t)reads;

            /* binary string --> char */
            c = (unsigned char)strtol(temp, NULL, number_base);

            LOG("Read %s = %d\n", temp, (int)c);

            /* insert char to string */
            darray_insert(ciphertext_seq[j], (void *)&c);
        }
        ++buf;
        ++i;
        ++j; /* next string */
    }

    file_buffer_destroy(fb);

    if (j != (size_t)num)
        ERROR("Can't Load all ciphertexts\n", NULL);

    *size = j;
    return ciphertext_seq;
}

int main(void)
{
    const Darray **ciphertext_seq;
    size_t size;
    size_t i;
    ciphertext_seq = (const Darray **)get_ciphertext_seq_from_file(path_to_ciphertext, &size);
    if (ciphertext_seq == NULL)
        return 1;

    /* we wanna decode last msg */
    SWAP(ciphertext_seq[0], ciphertext_seq[size - 1]);

    cipher_breaker_decode(ciphertext_seq, size);

     for (i = 0; i < size; ++i)
        darray_destroy(((Darray **)ciphertext_seq)[i]);

    FREE(ciphertext_seq);

    return 0;
}