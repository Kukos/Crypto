#include <aes.h>
#include <common.h>
#include <log.h>
#include <time.h>
#include <stdlib.h>
#include <generic.h>
#include <sys/stat.h>
#include <getch.h>
#include <unistd.h>
#include <filebuffer.h>
#include <openssl/evp.h>
#include <fcntl.h>
#include <base64.h>

#define AES_SECRET_KEY_ALIAS    "aes"
#define AES_SECRET_KEY_FILE     "./aes_secret_key"

#define AES_KEY_LEN     32
#define AES_IV_LEN      16
#define AES_BLOCK_SIZE  16

/*
    Generate IV

    PARAMS
    NO PARAMS

    RETURN
    NULL iff failure
    Ptr iff success
*/
static BYTE *aes_generate_iv(void);

/*
    Load secret key from java keytool

    PARAMS
    @IN passw - password or NULL

    RETURN
    Secret key
*/
static BYTE *aes_load_secret_key(const char *passw);

/*
    Core of aes encryption

    PARAMS
    @IN in - plaintext
    @IN key - key
    @IN iv - iv
    @IN mode - aes mode
    @OUT out - ciphertext

    RETURN
    Size of cipher iff success
    -1 iff failure

*/
static ssize_t __aes_encrypt(const BYTE *in, const BYTE *key, const BYTE *iv, const EVP_CIPHER *mode, BYTE **out);

/*
    Core of aes decryption

    PARAMS
    @IN in - ciphertext
    @IN len - len(in)
    @IN key - key
    @IN iv - iv
    @IN mode - aes mode
    @OUT out - plaintext
*/
static ssize_t __aes_decrypt(const BYTE *in, size_t in_len, const BYTE *key, const BYTE *iv, const EVP_CIPHER *mode, BYTE **out);

#define get_cipher_len(plaintext_len) (plaintext_len + AES_BLOCK_SIZE + (plaintext_len % AES_BLOCK_SIZE > 0 ? (AES_BLOCK_SIZE - plaintext_len % AES_BLOCK_SIZE) : 0))

static BYTE *aes_load_secret_key(const char *passw)
{
    const char *const file = "./aes_secret_key";
    struct stat dummy;
    char pass [AES_KEY_LEN] = {'\0'};
    static char aes_key[AES_KEY_LEN] = {'\0'};
    size_t index = 0;
    int c;
    char *cmd;

    FILE *keytool;

    TRACE();

    if (passw == NULL)
    {
        printf("Password:\n");
        while ((c = getch()) != 10)
        {
            pass[index++] = (char)c;
            putchar('*');
        }
        printf("\n");
        passw = pass;
    }

    if (stat(file, &dummy) != 0) /* create */
    {
        asprintf(&cmd, "java -classpath ./keytool_helper KeyStoreHelper -s -f %s -p %s -a %s", AES_SECRET_KEY_FILE, passw, AES_SECRET_KEY_ALIAS);
        if (system(cmd) == -1)
            FATAL("Cannot create secret key\n");

        FREE(cmd);
    }

    asprintf(&cmd, "java -classpath  ./keytool_helper KeyStoreHelper -g -f %s -p %s -a %s", AES_SECRET_KEY_FILE, passw, AES_SECRET_KEY_ALIAS);
    keytool = popen(cmd, "r");
    FREE(cmd);

    fscanf(keytool, "%s", aes_key);
    if (strcmp(aes_key, "fail") == 0)
        FATAL("Failed to read secret key\n");

    pclose(keytool);

    return (BYTE *)aes_key;
}

static BYTE *aes_generate_iv(void)
{
    size_t i;
    BYTE *iv;

    TRACE();

    iv = (BYTE *)malloc(AES_IV_LEN + 1);
    if (iv == NULL)
        ERROR("malloc error\n", NULL);

    srand((unsigned int)time(NULL));

    for (i = 0; i < AES_IV_LEN; ++i)
        iv[i] = (BYTE)((rand() % 255) + 1);

    iv[AES_IV_LEN] = '\0';

    return iv;
}

static ssize_t __aes_encrypt(const BYTE *in, const BYTE *key, const BYTE *iv, const EVP_CIPHER *mode, BYTE **out)
{
    EVP_CIPHER_CTX *ctx;
	size_t len = 0;
	size_t ciphertext_len = 0;
    size_t in_len = 0;

    BYTE *ptr;

    TRACE();

    if (in == NULL)
        ERROR("in == NULL\n", -1);

    if (key == NULL)
        ERROR("key == NULL\n", -1);

    if (iv == NULL)
        ERROR("iv == NULL\n", -1);

    if (mode == NULL)
        ERROR("mode == NULL\n", -1);

    if (out == NULL)
        ERROR("out == NULL\n", -1);

    in_len = strlen((const char *)in);

    *out = (BYTE *)malloc(get_cipher_len(in_len) + AES_IV_LEN + 1);
    if (*out == NULL)
        FATAL("malloc error\n");

    ptr = *out;
    (void)memcpy(ptr, iv, AES_IV_LEN);
    ciphertext_len = AES_IV_LEN;
    ptr += ciphertext_len;

	if ((ctx = EVP_CIPHER_CTX_new()) == NULL)
        ERROR("EVP_CIPHER_CTX_new error\n", -1);

	if (EVP_EncryptInit_ex(ctx, mode, NULL, (const unsigned char *)key, (const unsigned char *)iv) != 1)
        ERROR("EVP_EncryptInit_ex error\n", -1);

    EVP_CIPHER_CTX_set_padding(ctx, AES_BLOCK_SIZE);

	if (EVP_EncryptUpdate(ctx, (unsigned char *)ptr, (int *)&len, (const unsigned char *)in, (int)in_len) != 1)
        ERROR("EVP_EncryptUpdate error\n", -1);

	ciphertext_len += len;
    ptr += len;

	if (EVP_EncryptFinal_ex(ctx, (unsigned char *)ptr, (int *)&len) != 1)
        ERROR("EVP_EncryptFinal_ex error\n", -1);

    ciphertext_len += len;
    ptr += len;

	EVP_CIPHER_CTX_free(ctx);

    *ptr = '\0';
	return (ssize_t)ciphertext_len;
}

static ssize_t __aes_decrypt(const BYTE *in, size_t in_len, const BYTE *key, const BYTE *iv, const EVP_CIPHER *mode, BYTE **out)
{
    EVP_CIPHER_CTX *ctx;
	size_t len = 0;
	size_t plaintext_len = 0;

    BYTE *ptr;

    TRACE();

    if (in == NULL)
        ERROR("in == NULL\n", -1);

    if (key == NULL)
        ERROR("key == NULL\n", -1);

    if (iv == NULL)
        ERROR("iv == NULL\n", -1);

    if (mode == NULL)
        ERROR("mode == NULL\n", -1);

    if (out == NULL)
        ERROR("out == NULL\n", -1);

    *out = (BYTE *)malloc(in_len + 1 + 1);
    if (*out == NULL)
        FATAL("malloc error\n");

    ptr = *out;
	if ((ctx = EVP_CIPHER_CTX_new()) == NULL)
        ERROR("EVP_CIPHER_CTX_new error\n", -1);

    if (EVP_DecryptInit_ex(ctx, mode, NULL, (const unsigned char *)key, (const unsigned char *)iv) != 1)
        ERROR("EVP_DecryptInit_ex error\n", -1);

    EVP_CIPHER_CTX_set_padding(ctx, AES_BLOCK_SIZE);

    if (EVP_DecryptUpdate(ctx, (unsigned char *)ptr, (int *)&len, (const unsigned char *)in, (int)in_len) != 1)
        ERROR("EVP_DecryptUpdate error\n", -1);

	plaintext_len = len;
    ptr += len;

	if (EVP_DecryptFinal_ex(ctx, (unsigned char *)ptr, (int *)&len) != 1)
        ERROR("EVP_DecryptFinal_ex error\n", -1);

    plaintext_len += len;
    ptr += len;

	EVP_CIPHER_CTX_free(ctx);

    *ptr = '\0';
    return (ssize_t)plaintext_len;
}


ssize_t aes_encrypt(const BYTE *in, const BYTE *key, const BYTE *iv, const EVP_CIPHER *mode, BYTE **out)
{
    BYTE *_iv;
    ssize_t len;

    TRACE();

    _iv = (BYTE *)iv;
    if (_iv == NULL)
        _iv = aes_generate_iv();

    len = __aes_encrypt(in, key, _iv, mode, out);

    if (iv == NULL)
        FREE(_iv);

    return len;
}

ssize_t aes_decrypt(const BYTE *in, size_t in_len, const BYTE *key, const EVP_CIPHER *mode, BYTE **out)
{
    BYTE iv[AES_IV_LEN + 1];

    TRACE();

    (void)memcpy(iv, in, AES_IV_LEN);
    iv[AES_IV_LEN] = '\0';

    return __aes_decrypt(in + AES_IV_LEN, in_len - AES_IV_LEN, key, iv, mode, out);
}

int aes_encrypt_file(const char *in, const char *out, const BYTE *iv, const EVP_CIPHER *mode, const char *passw)
{
    BYTE *aes_secret_key;
    BYTE *cipher;
    BYTE *base64;
    ssize_t cipher_len;
    File_buffer *fb;
    File_buffer *fb2;

    TRACE();

    fb = file_buffer_create_from_path(in, PROT_READ | PROT_WRITE, O_RDWR);
    if (fb == NULL)
        ERROR("file_buffer_create_from_path error\n", 1);

    aes_secret_key = aes_load_secret_key(passw);

    cipher_len = aes_encrypt((BYTE *)file_buffer_get_buff(fb), aes_secret_key, iv, mode, &cipher);
    if (cipher_len == -1)
        ERROR("aes_encrypt error\n", 1);

    fb2 = file_buffer_create_from_path(out, PROT_READ | PROT_WRITE | PROT_EXEC, O_RDWR | O_CREAT | O_TRUNC);
    if (fb2 == NULL)
        ERROR("file_buffer_create_from_path error\n", 1);

    base64 = base64_encode(cipher, (size_t)cipher_len, NULL);
    file_buffer_append(fb2, (char *)base64);
    file_buffer_synch(fb2);

    file_buffer_destroy(fb);
    file_buffer_destroy(fb2);

    FREE(cipher);
    FREE(base64);

    return 0;
}

int aes_decrypt_file(const char *in, const char *out, const EVP_CIPHER *mode, const char *passw)
{
    BYTE *aes_secret_key;
    BYTE *plaintext;
    ssize_t plaintext_len;
    File_buffer *fb;
    File_buffer *fb2;

    size_t rsize;
    BYTE *base64;
    TRACE();

    fb = file_buffer_create_from_path(in, PROT_READ | PROT_WRITE, O_RDWR);
    if (fb == NULL)
        ERROR("file_buffer_create_from_path error\n", 1);

    aes_secret_key = aes_load_secret_key(passw);
    base64 = base64_decode((BYTE *)file_buffer_get_buff(fb), (size_t)file_buffer_get_size(fb), &rsize);

    plaintext_len = aes_decrypt(base64, rsize, aes_secret_key, mode, &plaintext);
    if (plaintext_len == -1)
        ERROR("aes_decrypt error\n", 1);

    fb2 = file_buffer_create_from_path(out, PROT_READ | PROT_WRITE | PROT_EXEC, O_RDWR | O_CREAT | O_TRUNC);
    if (fb2 == NULL)
        ERROR("file_buffer_create_from_path error\n", 1);

    FREE(base64);

    file_buffer_append(fb2, (char *)plaintext);
    file_buffer_synch(fb2);

    file_buffer_destroy(fb);
    file_buffer_destroy(fb2);

    FREE(plaintext);

    return 0;
}