/*
 * Created: Sáb 27 Nov 2010 08:50:55 BRST
 *
 * Author: Maycon Sambinelli, msambinelli@gmail.com
 *
 */
#include "util.h"
#include <sys/resource.h>
 
void *Malloc(size_t size){
    void *p = malloc(size);
    if (!p){
        perror("malloc");       
        abort();
    }
    return p;
}

double current_user_time_secs(void){
    double usertime, systemtime;
    struct rusage usage;

    if (getrusage(RUSAGE_SELF, &usage) < 0){
        perror("getrusage");
        return -1;
    }
    usertime = usage.ru_utime.tv_sec + (usage.ru_utime.tv_usec * 1e-6);
    systemtime = usage.ru_stime.tv_sec + (usage.ru_stime.tv_usec * 1e-6);
    return (usertime + systemtime);
}

long int seed_prng()
{
    long int gseed;
    struct timeval tval;

    if (gettimeofday(&tval, NULL) < 0) {
        perror("gettimeofday");
        return 1;
    }
    gseed = tval.tv_sec * tval.tv_usec;

    return gseed;
}

