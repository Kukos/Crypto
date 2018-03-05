#ifndef MRAND_BREAKER_H
#define MRAND_BREAKER_H

/*
    Implementation of breaker the mrand function

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL3.0+
*/

/*
    Observe and collect needed data to predict

    PARAMS
    @IN _rand - pointer to rand function

    RETURN
    This is a void function
*/
void mrand_breaker_observe(long (*_rand)(void));

/*
    After observations predict next number

    PARAMS
    NO PARAMS

    RETURN
    -1 iff breaker is not ready
    Next predicted number iff ready
*/
long mrand_breaker_predict_next(void);

/*
    Add new state to prediction

    PARAMS
    @IN state - new state

    RETURN
    This is a void function
*/
void mrand_breaker_add_state(long state);

#endif