#ifndef COMMON_H
#define COMMON_H


#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#define PARAM_Q 3329
#define PARAM_K 12
#define PARAM_N 256
#define PARAM_D 2
#if PARAM_D==1
    #define PARAM_SIGMA_L 7
#elif PARAM_D==2
    #define PARAM_SIGMA_L 4
#elif PARAM_D==3
    #define PARAM_SIGMA_L 4
#elif PARAM_D==4
    #define PARAM_SIGMA_L 3
#else
    #error "Unsupported value for PARAM_D"
#endif
        
#define PARAM_M PARAM_D*(PARAM_K+2) //2*(PARAM_D+1)*PARAM_K
#define PARAM_SIGMA 7.00
#define NEEDED_AES_FOR_SALT 3
#define PARAM_ETA1 2
#define PARAM_ETA2 2
#define PARAM_Du 11
#define PARAM_Dv 5
typedef long double RR_t;
typedef uint32_t scalar;

#endif