#ifndef LCG_BREAKER_H
#define LCG_BREAKER_H

/*
    Implementation of lcg breaker

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL3.0+
*/

/*
    Observe and collect data needed to break LCG

    PARAMS
    @IN _lcg_rand - pointer to lcg rand function

    RETURN
    This is a void function
*/
void lcg_breaker_observe(long (*_lcg_rand)(void));

/*
    Predict next lcg number

    PARAMS
    NO PARAMS

    RETURN
    -1 iff breaker is not ready
    Next predicted number iff ready
*/
long lcg_breaker_predict_next(void);

#endif