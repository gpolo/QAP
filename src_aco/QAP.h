/*
 * Created: Qua 24 Nov 2010 17:30:01 BRST
 *
 * Author: Maycon Sambinelli, msambinelli@gmail.com
 *
 */
#ifndef __QAP_H
#define __QAP_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define N 257

#define PHEUP_IB    1
#define PHEUP_BF    2
#define SYMMETRIC   8
#define ASYMMETRIC  16
#define TIMESTOP    32
#define BESTSTOP    64
#define VERBOSE     128
#define MGENERATION 256

typedef struct QAP_t QAP_t;
typedef struct QAP_solution_t QAP_solution_t;

struct QAP_t {
    int flow[N][N];
    int dist[N][N];
    int size;
    int nants;
    float phero_pers; /* pheromone persistence */
    int ngenerations;
    unsigned flags;
    int best_know_solution;
    double time_limit;
};

struct QAP_solution_t {
    int perm[N];
    int cst;
    int geration;
};

int QAP_calc_cst(QAP_t *prob, QAP_solution_t *sol);
void QAP_info(QAP_t *prob);
int QAP_check_solution(QAP_t *prob, QAP_solution_t *sol);
int QAP_check_association(QAP_t *prob, QAP_solution_t *sol);
void QAP_show_solution(QAP_t *prob, QAP_solution_t *sol);
 
#endif /* __QAP_H */
