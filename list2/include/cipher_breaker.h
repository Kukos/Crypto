#ifndef CIPHER_BREAKER_H
#define CIPHER_BREAKER_H

/*
    Decode orginal msg from seq of cipher encoded with the same unknown key

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL 3.0
*/

#include <stddef.h>
#include <darray.h>

/*
    From seq of encoded ciphertext with the same key
    print on stdout every correct solution for the first msg

    PARAMS
    @IN ciphertext_seq - array of ciphertext
    @IN len - len(ciphertext_seq)

    RETURN
    This is a void function
*/
void cipher_breaker_decode(const Darray* const *ciphertext_seq, size_t len);


#endif