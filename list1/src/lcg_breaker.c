#include <lcg_breaker.h>
#include <stdbool.h>
#include <stddef.h>
#include <common.h>
#include <log.h>

#define NEEDED_STATES 10

static long _t[NEEDED_STATES];
static bool _is_ready;

static long _state;
static long _add;
static long _mul;
static long _mod;

/* FIND PARAMS */
static long find_add_param(void);
static long find_mul_param(void);
static long find_mod_param(void);

/*
    Euclidean Algorithm

    PARAMS
    @IN a - first number
    @IN b - second number

    RETURN
    gcd(a, b)
*/
static long gcd(long a, long b);

/*
    Extended Euclidean Algorithm
    See: https://en.wikibooks.org/wiki/Algorithm_Implementation/Mathematics/Extended_Euclidean_algorithm#Modular_inverse

    ax + by = gcd(a, b)

    PARAMS
    @IN a - first number
    @IN b - second number
    @OUT x - x from formula
    @OUT y - y from formula

    RETURN
    GCD(a, b) also produce x and y
*/
static long egcd(long a, long b, long *x, long *y);

/*
    Compute a^-1 (mod m)

    PARAMS
    @IN a - number to inverse
    @IN m - modulo

    RETURN
    -1 iff not exist a^-1 (mod m)
    a^-1 (mod m) iff exist
*/
static long inverse_modulo(long a, long m);

static long gcd(long a, long b)
{
    long c;

    TRACE();
    while (b)
    {
        c = a % b;
        a = b;
        b = c;
    }

    return a;
}

static long egcd(long a, long b, long *x, long *y)
{
    long c = 0;
    long d = 1;
    long u = 1;
    long v = 0;
    long q;
    long m;
    long r;
    long n;
    long _a = a;
    long _b = b;

    TRACE();

    while (_a)
    {
        q = _b / _a;
        r = _b % _a;
        m = c - u * q;
        n = d - v * q;
        _b = _a;
        _a = r;
        c = u;
        d = v;
        u = m;
        v = n;
    }

    LOG("%ld * %ld + %ld * %ld = gcd(%ld, %ld)\n", a, c, b, d, a, b);

    *x = c;
    *y = d;
    return _b;
}

static long inverse_modulo(long a, long m)
{
    long x;
    long y;
    long gcd;

    TRACE();

    gcd = egcd(a, m, &x, &y);
    (void)y;

    if (gcd == 1)
        return x % m;
    else
        return -1;
}

static long find_mod_param(void)
{
    /*
        A[0] = t[1] - t[0]
        A[1] = t[2] - t[1] = (t[1] * mul + add) - (t[0] * mul + add) = mul * (t[1] - t[0]) = mul * A[0] (mod n)
        ...
        A[2] * A[0] - A[1]^2 = (mul * mul * A[0] * A[0]) - (mul * A[0] * mul * A[0]) = 0 (mod n)
        ...
    */
    long diffs[NEEDED_STATES - 1];
    long congruents[NEEDED_STATES - 3];
    size_t i;
    long _gcd;

    TRACE();

    for (i = 0; i < ARRAY_SIZE(diffs); ++i)
        diffs[i] = _t[i + 1] - _t[i];

    for (i = 0; i < ARRAY_SIZE(congruents); ++i)
        congruents[i] = diffs[i + 2] * diffs[i] - diffs[i + 1] * diffs[i + 1];

    _gcd = congruents[0];
    for (i = 1; i < ARRAY_SIZE(congruents); ++i)
        _gcd = gcd(_gcd, congruents[i]);

    return ABS(_gcd);
}

static long find_add_param(void)
{
    TRACE();

	/*
        t[1] = (t[0] * mul + add) (mod n) ==> add = (t[1] - t[0] * mul) (mod n)
    */
    return ((_t[1] - _t[0] * _mul) % _mod + _mod) % _mod;
}

static long find_mul_param(void)
{
	/*
        t[1] = (t[0] * mul + add) (mod n)
        t[2] = (t[1] * mul + add) (mod n)

        t[2] - t[1] = mul * (t[1] - t[0]) % mod ==> mul = (t[2] - t[1]) / (t[1] - t[0])	(mod n)
	*/
    long diff1 = _t[2] - _t[1];
    long diff2_inv = inverse_modulo(_t[1] - _t[0], _mod);

    TRACE();

    return ((diff1 * diff2_inv) % _mod + _mod) % _mod;
}

void lcg_breaker_observe(long (*_lcg_rand)(void))
{
    size_t i;

    TRACE();

    LOG("Observing %d times\n", NEEDED_STATES);
    for (i = 0; i < NEEDED_STATES; ++i)
        _t[i] = _lcg_rand();

    _state = _t[NEEDED_STATES - 1];

    LOG("Computing params\n");
    _mod = find_mod_param();
    _mul = find_mul_param();
    _add = find_add_param();

    LOG("PARAMS computed, Mul = %ld, Add = %ld, Mod = %ld\n", _mul, _add, _mod);

    _is_ready = true;
}

long lcg_breaker_predict_next(void)
{
    TRACE();

    if (!_is_ready)
        ERROR("LCG Breaker is not ready\n", -1);

    _state = ((_state * _mul + _add) % _mod + _mod) % _mod;
    return _state;
}
