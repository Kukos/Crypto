#ifndef LCG_H
#define LCG_H

/*
    Simple implementation of linear congruential generator

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL3.0+
*/

/*
    Prepare lcg to randomize number by giver formula:
    next = (prev * mul + add) % mod

    PARAMS
    @IN seed - random seed
    @IN mul - mul param in formula
    @IN add - add param in formula
    @IN mod - mod param in formula

    RETURN
    This is a void function
*/
void lcg_init(long seed, long mul, long add, long mod);

/*
    Get next random number

    PARAMS
    NO PARAMS

    RETURN
    -1 iff lcg is not ready
    Next random iff is ready
*/
long lcg_rand(void);

#endif