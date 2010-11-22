/*
 * Created:  22/11/2010 10:23:55
 *
 * Author:  Guilherme Polo, ggpolo@gmail.com
 *
 */
#ifndef MEMETIC_H
#define MEMETIC_H

#define DEBUG 0

#define POPSIZE 50
#define INITIAL_POPSIZE 42
#define RECOMBINATIONS 30
#define MUTATE_XGENS_STAGNATE (POPSIZE / 2 + 5)
#define GENERATIONS (MUTATE_XGENS_STAGNATE * 111)

struct Individual {
    int *data;
    int cost;
};

int memetic(int, int);

#endif