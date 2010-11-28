/*
 * Created:  28/11/2010 00:47:46
 *
 * Author:  Guilherme Polo, ggpolo@gmail.com
 *
 */
#ifndef FLAGS_H
#define FLAGS_H

#define VERBOSE     2
#define SYMMETRIC   4
#define ASYMMETRIC  8
#define MEMETIC    16
#define TIME_LIMIT 32
#define SET_SEED   64

#define ARTIFICIAL  1

struct ParseResult {
    int flags;
    int artificial_cost_limit;
    long int seed;
    double time_limit;
};


#endif
