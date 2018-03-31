#ifndef AES_H
#define AES_H

/*
    Implementation of encryption / decryption using aes256 algo

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE GPL3.0
*/

#include <generic.h>
#include <openssl/evp.h>
#include <sys/types.h>

/*
    Encrypt bytes with aes

    PARAMS
    @IN in - plaintext
    @IN key - key
    @IN iv - iv
    @IN mode - mode
    @OUT out - cipher text

    RETURN
    Size of ciphertext iff success
    -1 iff failure
*/
ssize_t aes_encrypt(const BYTE *in, const BYTE *key, const BYTE *iv, const EVP_CIPHER *mode, BYTE **out);

#define aes_encrypt_cbc(in, key, iv, out) \
    __extension__ \
    ({ \
        EVP_CIPHER ____evp_cipher_mode = *(EVP_CIPHER *)EVP_aes_256_cbc(); \
        ssize_t ____size = aes_encrypt(in, key, iv, &____evp_cipher_mode, out); \
        ____size; \
    })

#define aes_encrypt_ctr(in, key, iv, out) \
    __extension__ \
    ({ \
        EVP_CIPHER ____evp_cipher_mode = *(EVP_CIPHER *)EVP_aes_256_ctr(); \
        ssize_t ____size = aes_encrypt(in, key, iv, &____evp_cipher_mode, out); \
        ____size; \
    })

#define aes_encrypt_ofb(in, key, iv, out) \
    __extension__ \
    ({ \
        EVP_CIPHER ____evp_cipher_mode = *(EVP_CIPHER *)EVP_aes_256_ofb(); \
        ssize_t ____size = aes_encrypt(in, key, iv, &____evp_cipher_mode, out); \
        ____size; \
    })

/*
    Encrypt file using AES

    PARAMS
    @IN in - file to encrypt
    @IN out - encrypted file
    @IN iv - iv
    @IN mode - mode
    @IN passw - password

    RETURN
    0 iff success
    Non-zero value iff failure
*/
int aes_encrypt_file(const char *in, const char *out, const BYTE *iv, const EVP_CIPHER *mode, const char *passw);

#define aes_encrypt_file_cbc(in, out, iv, pass) \
    __extension__ \
    ({ \
        EVP_CIPHER ____evp_cipher_mode = *(EVP_CIPHER *)EVP_aes_256_cbc(); \
        ssize_t ____size = aes_encrypt_file(in, out, iv, &____evp_cipher_mode, pass); \
        ____size; \
    })

#define aes_encrypt_file_ctr(in, out, iv, pass) \
    __extension__ \
    ({ \
        EVP_CIPHER ____evp_cipher_mode = *(EVP_CIPHER *)EVP_aes_256_ctr(); \
        ssize_t ____size = aes_encrypt_file(in, out, iv, &____evp_cipher_mode, pass); \
        ____size; \
    })

#define aes_encrypt_file_ofb(in, out, iv, pass) \
    __extension__ \
    ({ \
        EVP_CIPHER ____evp_cipher_mode = *(EVP_CIPHER *)EVP_aes_256_ofb(); \
        ssize_t ____size = aes_encrypt_file(in, out, iv, &____evp_cipher_mode, pass); \
        ____size; \
    })


/*
    Decrypt bytes with aes

    PARAMS
    @IN in - ciphertext
    @IN len - len
    @IN key - key
    @IN iv - iv
    @IN mode - mode
    @OUT out - plain text

    RETURN
    Size of plaintext iff success
    -1 iff failure
*/
ssize_t aes_decrypt(const BYTE *in, size_t in_len, const BYTE *key, const EVP_CIPHER *mode, BYTE **out);

#define aes_decrypt_cbc(in, len, key, out) \
    __extension__ \
    ({ \
        EVP_CIPHER ____evp_cipher_mode = *(EVP_CIPHER *)EVP_aes_256_cbc(); \
        ssize_t ____size = aes_decrypt(in, len, key, &____evp_cipher_mode, out); \
        ____size; \
    })

#define aes_decrypt_ctr(in, len, key, out) \
    __extension__ \
    ({ \
        EVP_CIPHER ____evp_cipher_mode = *(EVP_CIPHER *)EVP_aes_256_ctr(); \
        ssize_t ____size = aes_decrypt(in, len, key, &____evp_cipher_mode, out); \
        ____size; \
    })

#define aes_decrypt_ofb(in, len, key, out) \
    __extension__ \
    ({ \
        EVP_CIPHER ____evp_cipher_mode = *(EVP_CIPHER *)EVP_aes_256_ofb(); \
        ssize_t ____size = aes_decrypt(in, len, key, &____evp_cipher_mode, out); \
        ____size; \
    })


/*
    Decrypt file using AES

    PARAMS
    @IN in - file to decrypt
    @IN out - decrypted file
    @IN mode - mode
    @IN passw - password

    RETURN
    0 iff success
    Non-zero value iff failure
*/
int aes_decrypt_file(const char *in, const char *out, const EVP_CIPHER *mode, const char *passw);

#define aes_decrypt_file_cbc(in, out, pass) \
    __extension__ \
    ({ \
        EVP_CIPHER ____evp_cipher_mode = *(EVP_CIPHER *)EVP_aes_256_cbc(); \
        ssize_t ____size = aes_decrypt_file(in, out, &____evp_cipher_mode, pass); \
        ____size; \
    })

#define aes_decrypt_file_ctr(in, out, pass) \
    __extension__ \
    ({ \
        EVP_CIPHER ____evp_cipher_mode = *(EVP_CIPHER *)EVP_aes_256_ctr(); \
        ssize_t ____size = aes_decrypt_file(in, out, &____evp_cipher_mode, pass); \
        ____size; \
    })

#define aes_decrypt_file_ofb(in, out, pass) \
    __extension__ \
    ({ \
        EVP_CIPHER ____evp_cipher_mode = *(EVP_CIPHER *)EVP_aes_256_ofb(); \
        ssize_t ____size = aes_decrypt_file(in, out, &____evp_cipher_mode, pass); \
        ____size; \
    })


#endif