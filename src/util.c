/*
 * Created:  23/08/2010 18:16:40
 *
 * Author:  Guilherme Polo, ggpolo@gmail.com
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/time.h>
#include "util.h"


void *
Malloc(size_t size)
{
    void *ptr = malloc(size);
    if (!ptr) {
        perror("malloc");
        abort();
    }
    return ptr;
}

void
show_sol(int *sol, int length, int cost)
{
    int i;

    for (i = 0; i < length; i++) {
        printf("%d ", sol[i]);
    }
    printf(": %d\n", cost);
}

double
current_usertime_secs(void)
{
    double usertime, systemtime;
    struct rusage usage;

    if (getrusage(RUSAGE_SELF, &usage) < 0) {
        perror("getrusage");
        return -1;
    }
    usertime = usage.ru_utime.tv_sec + (usage.ru_utime.tv_usec * 1e-6);
    systemtime = usage.ru_stime.tv_sec + (usage.ru_stime.tv_usec * 1e-6);
    return (usertime + systemtime);
}

int
seed_prng(long int seed, int flag, long int *genseed)
{
    long int gseed;
    struct timeval tval;

    if (flag & SEED_NEW) {
        if (gettimeofday(&tval, NULL) < 0) {
            perror("gettimeofday");
            return 1;
        }
        gseed = tval.tv_sec * tval.tv_usec;
    } else {
        gseed = seed;
    }

    srand48(gseed);
    if (genseed != NULL)
        *genseed = gseed;

    return 0;
}

void
random_shuffle(int *src, int n)
{
    int i, j, temp;

    for (i = n - 1; i > 0; i--) {
        j = (int)(RANDOM_UNIT() * (i + 1));
        temp = src[i];
        src[i] = src[j];
        src[j] = temp;
    }
}

int
distance(int *x, int *y, int n)
{
    int i, d = 0;

    for (i = 0; i < n; i++) {
        if (x[i] != y[i])
            d++;
    }
    return d;
}
