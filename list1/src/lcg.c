#include <lcg.h>
#include <stdbool.h>
#include <log.h>

static long _state;
static long _mul;
static long _add;
static long _mod;
static bool _is_ready = false;


void lcg_init(long seed, long mul, long add, long mod)
{
    TRACE();

    LOG("Setting parameters: Seed = %ld, Mul = %ld, Add = %ld, Mod = %ld\n", seed, mul, add, mod);
    _state = seed;
    _mul = mul;
    _add = add;
    _mod = mod;

    _is_ready = true;
}

long lcg_rand(void)
{
    TRACE();

    if (!_is_ready)
        ERROR("LCG is not ready\n", -1);

    _state = ((_state * _mul + _add) % _mod + _mod) % _mod;

    return _state;
}