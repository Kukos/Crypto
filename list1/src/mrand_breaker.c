#include <mrand_breaker.h>
#include <stdbool.h>
#include <stddef.h>
#include <common.h>
#include <log.h>

#define NEEDED_STATES 31

static long _t[NEEDED_STATES];
static bool _is_ready = false;
static size_t _index = 0;

void mrand_breaker_observe(long (*_rand)(void))
{
    size_t i;

    TRACE();

    LOG("Observing %d times\n", NEEDED_STATES);

    for (i = 0; i < NEEDED_STATES; ++i)
        mrand_breaker_add_state(_rand());

    LOG("DONE ready for predicting\n");

    _is_ready = true;
}

long mrand_breaker_predict_next(void)
{
    long number;

    TRACE();

    if (!_is_ready)
        ERROR("Breaker is not ready\n", -1);

    number = (_t[(_index - NEEDED_STATES) % NEEDED_STATES] + _t[(_index - 3) % NEEDED_STATES]) % (long)BIT(31);
    LOG("Predicting using formula: t[state - %d] + t[state - 3] 2^31\nNumber = %ld\n", NEEDED_STATES, number);

    return number;
}

void mrand_breaker_add_state(long state)
{
    TRACE();

    _t[_index % NEEDED_STATES] = state;
    ++_index;
}