#include <mrand.h>
#include <stdlib.h>
#include <log.h>

void msrand(unsigned int seed)
{
    TRACE();
    srandom(seed);
}

long mrand(void)
{
    TRACE();
    return random();
}