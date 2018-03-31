#include <aes.h>
#include <stdio.h>
#include <stdlib.h>
#include <log.h>
#include <common.h>
#include <generic.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <openssl/rand.h>
#include <time.h>

#define CPA_IV_LEN  QWORD_SIZE
#define CPA_MSG_LEN QWORD_SIZE

static const BYTE key[] = "Kukos";

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

/* chosen plain text attack  */
bool cpa(void);

/* get 2msg, encrypt 1 */
int oracle(const BYTE *m1, const BYTE *m2, const BYTE *iv, BYTE **out);

int oracle(const BYTE *m1, const BYTE *m2, const BYTE *iv, BYTE **out)
{
    int r = rand();

    EVEN(r) ? ((void)aes_encrypt_cbc(m1, key, iv, out)) : ((void)aes_encrypt_cbc(m2, key, iv, out));

    return EVEN(r);
}


bool cpa(void)
{
    BYTE msg1[CPA_MSG_LEN + 1] = {0};
    BYTE msg2[CPA_MSG_LEN + 1] = {0};
    BYTE *c1;
    BYTE *c2;
    BYTE *c3;
    QWORD iv;

    int b;
    int my_b;

    /* encrypt msg to get IV */
    (void)aes_encrypt_cbc(msg1, key, NULL, &c1);
    iv = *(QWORD *)c1;

    ++iv;

    /* send msg = iv + 1 to get F_k(0) */
    (void)memcpy(msg1, &iv, QWORD_SIZE);

    (void)aes_encrypt_cbc(msg1, key, (BYTE *)&iv, &c2);
    c2 += CPA_IV_LEN;

    ++iv;
    /* now we can create 2M, 1M = IV, 2M = random and we will know which msg was encrypted */

    /* send msg = iv + 1 to get F_k(0) */
    (void)memcpy(msg1, &iv, QWORD_SIZE);

    /* send rand msg */
    RAND_bytes(msg2, CPA_MSG_LEN);

    b = oracle(msg1, msg2, (BYTE *)&iv, &c3);
    c3 += CPA_IV_LEN;

    my_b = (strcmp((char *)c2, (char *)c3) == 0);

    return b == my_b;
}

int main(void)
{
    size_t i;
    const size_t n = BIT(20);
    size_t correct = 0;

    srand((unsigned)time(NULL));

    for (i = 0; i < n; ++i)
        correct += cpa();

    printf("CORRECT %zu / %zu\n", correct, n);

    return 0;
}