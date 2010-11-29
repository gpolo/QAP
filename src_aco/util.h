/*
 * Created: Sáb 27 Nov 2010 08:54:44 BRST
 *
 * Author: Maycon Sambinelli, msambinelli@gmail.com
 *
 */
#ifndef __UTIL_H
#define __UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define RANDOM(limit) (lrand48() % (limit))
#define RANDOM_UNIT() (lrand48())

void *Malloc(size_t);
double current_user_time_secs(void);
long int seed_prng();

#endif /* __UTIL_H */

