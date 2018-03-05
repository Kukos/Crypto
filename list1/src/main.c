#include <mrand.h>
#include <mrand_breaker.h>
#include <lcg.h>
#include <lcg_breaker.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <common.h>
#include <compiler.h>
#include <log.h>

#define TESTS 1000000

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

/* BREAKERS TESTS */
static void mrand_breaker_test(size_t test);
static void lcg_breaker_test(size_t test);

static void mrand_breaker_test(size_t test)
{
    size_t i;
    size_t passed = 0;
    long randed;
    long predicted;

    msrand((unsigned int)time(NULL));
    mrand_breaker_observe(mrand);

    for (i = 0; i < test; ++i)
    {
        randed = mrand();
        predicted = mrand_breaker_predict_next();
        mrand_breaker_add_state(randed);
        if (randed == predicted)
            ++passed;
    }

    printf("MRAND TEST: PASSED %zu / %zu\n", passed, test);
}

static void lcg_breaker_test(size_t test)
{
    size_t i;
    size_t passed = 0;
    long randed;
    long predicted;

    const long mul = 1103515245;
	const long add = 12345;
	const long mod = (long)BIT(31);
    const long seed = 235111741;

    lcg_init(seed, mul, add, mod);
    lcg_breaker_observe(lcg_rand);

    for (i = 0; i < test; ++i)
    {
        randed = lcg_rand();
        predicted = lcg_breaker_predict_next();
        if (randed == predicted)
            ++passed;
    }

    printf("LCG TEST: PASSED %zu / %zu\n", passed, test);
}

int main(void)
{
    lcg_breaker_test(TESTS);
    mrand_breaker_test(TESTS);

    return 0;
}