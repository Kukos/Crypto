#include <stdio.h>
#include <string.h>
#include <log.h>
#include <stdlib.h>
#include <common.h>
#include <aes.h>

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

typedef enum
{
    AES_MODE_ENCRYPTION,
    AES_MODE_DECRYPTION,
} aes_mode_t;

static void help(void)
{
    printf("\t\tAES Enc / Dec \n");
    printf("PARAMS\n");
    printf("--encrypt / -e - encrypt file\n");
    printf("--decrypt / -d - decrypt file\n");
    printf("file\n");
    printf("--cbc / --ctr / --ofb\n");
}

#define create_new_file_name(out, file, mode) \
    do { \
        const char *const ext = mode == AES_MODE_ENCRYPTION ? ".enc" : ".dec"; \
        char *ptr = strrchr(file, '.'); \
        if (ptr != NULL && *(ptr + 1) != '/') \
            *ptr = '\0'; \
        (void)asprintf(&out, "%s%s", file, ext); \
        if (ptr != NULL && *(ptr + 1) != '/') \
            *ptr = '.'; \
    } while (0)

int main(int argc, char **argv)
{
    const char *file;
    char *output;
    aes_mode_t mode;

    if (argc < 4)
    {
        help();
        return 1;
    }

    if (strcmp(argv[1], "--encrypt") == 0 || strcmp(argv[1], "-e") == 0)
        mode = AES_MODE_ENCRYPTION;
    else if (strcmp(argv[1], "--decrypt") == 0 || strcmp(argv[1], "-d") == 0)
        mode = AES_MODE_DECRYPTION;
    else
    {
        help();
        return 1;
    }

    file = argv[2];
    create_new_file_name(output, file, mode);

    switch (mode)
    {
        case AES_MODE_DECRYPTION:
        {
            if (strcmp(argv[3], "--cbc") == 0)
                aes_decrypt_file_cbc(file, output, NULL);
            else if (strcmp(argv[3], "--ctr") == 0)
                aes_decrypt_file_ctr(file, output, NULL);
            else if (strcmp(argv[3], "--ofb") == 0)
                aes_decrypt_file_ofb(file, output, NULL);

            break;
        }
        case AES_MODE_ENCRYPTION:
        {
            if (strcmp(argv[3], "--cbc") == 0)
                aes_encrypt_file_cbc(file, output, NULL, NULL);
            else if (strcmp(argv[3], "--ctr") == 0)
                aes_encrypt_file_ctr(file, output, NULL, NULL);
            else if (strcmp(argv[3], "--ofb") == 0)
                aes_encrypt_file_ofb(file, output, NULL, NULL);

            break;
        }
        default:
        {
            break;
        }
    }

    FREE(output);
    return 0;
}