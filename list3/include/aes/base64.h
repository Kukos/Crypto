#ifndef BASE64_H
#define BASE64_H

/*
    Implemenatation of well known base64 coding

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE GPL3.0
*/

#include <generic.h>
#include <stddef.h>

/*
    Encode bytes to base64 coding

    PARAMS
    @IN bytes - array
    @IN len - len(bytes)
    @OUT size - encoded bytes array size

    RETURN
    Ptr to encoded bytes iff success
    NULL iff failure
*/
BYTE *base64_encode(const BYTE *bytes, size_t len, size_t *size);

/*
    Decode bytes from base64 coding

    PARAMS
    @IN bytes - array
    @IN len - len(bytes)
    @OUT size - decoded bytes array size

    RETURN
    Ptr to decoded bytes iff success
    NULL iff failure
*/
BYTE *base64_decode(const BYTE *bytes, size_t len, size_t *size);

#endif