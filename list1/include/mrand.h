#ifndef MRAND_H
#define MRAND_H

/*
    Wrapper for known rand algo in this case glibc random

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL 3.0
*/

/*
    Set seed to random

    PARAMS
    @IN seed - random seed

    RETURN
    This is a void function
*/
void msrand(unsigned int seed);

/*
    Get new random number

    PARAMS
    NO PARAMS

    RETURN
    New random number
*/
long mrand(void);

#endif