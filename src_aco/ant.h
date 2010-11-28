/*
 * Created: Qua 24 Nov 2010 18:28:10 BRST
 *
 * Author: Maycon Sambinelli, msambinelli@gmail.com
 *
 */
#ifndef __ANT_H
#define __ANT_H

#include <limits.h>

#include "QAP.h"

#define FLOW(prob, sol, lct1, lct2) ((prob)->flow[(sol)->perm[(lct1)]][(sol)->perm[(lct2)]]) 

typedef struct set_t set_t;
typedef int (*calc_reduction_exchange)(QAP_t *, QAP_solution_t *, int , int );

struct set_t {
    int data[N];
    int size;
};

QAP_solution_t *ant_build_solution(QAP_t *p);
void ant_system_init(QAP_t *prob);
void ant_construct_solutions(QAP_t *prob);
void aco_update_pheromones(QAP_t *prob);
QAP_solution_t *aco(QAP_t *prob);
void aco_local_search(QAP_t *prob);
 
#endif /* __ANT_H */

