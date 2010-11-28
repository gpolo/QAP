/*
 * Created:  23/08/2010 18:17:19
 *
 * Author:  Guilherme Polo, ggpolo@gmail.com
 *
 */
#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>

#define MATRIX2D(x) (&x[0][0])
#define RANDOM(limit) (lrand48() % (limit))
#define RANDOM_UNIT() (drand48())
#define SEED_REUSE 0
#define SEED_NEW 1

void *Malloc(size_t);
int Scanf(int, const char *, ...);

int seed_prng(long int, int, long int *);
void show_sol(int *, int, int);
double current_usertime_secs(void);
void random_shuffle(int *, int);
int distance(int *, int *, int);

#endif /* UTIL_H */
