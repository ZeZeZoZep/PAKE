#include <inttypes.h>
#include <x86intrin.h>

#define PARAM_Q 1073734913
//#define PARAM_N 256
//#define PARAM_D 2
#define PARAM_SIGMA 7.00
#define NEEDED_AES_FOR_SALT 3

typedef long double RR_t;
typedef uint32_t scalar;
uint32_t uniform_int_distribution(uint32_t n);

scalar uniform_mod_q(void);

/*
	Code from random_aesni.c
*/

//public API
void random_bytes_init(void);

//void random_bytes(uint8_t * restrict data);

/*
	Code from exp_aes.cpp
*/

double algorithm_EA(uint64_t * n);

/*
	Code from algoF_aes.cpp
*/
#ifdef __cplusplus
extern "C" {
#endif
int algorithmF(const double mu, const double sigma);
#ifdef __cplusplus
}
#endif


void salt(uint8_t *r);