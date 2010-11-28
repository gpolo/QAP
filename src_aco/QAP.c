/*
 * Created: Qua 24 Nov 2010 18:21:39 BRST
 *
 * Author: Maycon Sambinelli, msambinelli@gmail.com
 *
 */

#include "QAP.h"

int QAP_calc_cst(QAP_t *prob, QAP_solution_t *sol){
    
    int i, j;
    long long int cst = 0;
    for (i = 0; i < prob->size; i++){
        for (j = 0; j < prob->size; j++){
              cst += prob->dist[i][j] * prob->flow[sol->perm[i]][sol->perm[j]];
        }
    }
    return cst;
}

int QAP_check_association(QAP_t *prob, QAP_solution_t *sol){
    int i;
    int count[N];

    for (i = 0; i < prob->size; i++) count[i] = 0;
    for (i = 0; i < prob->size; i++) count[sol->perm[i]]++;
    for (i = 0; i < prob->size; i++) 
        if (count[i] != 1)
            return -1;

    return 1;
}

int QAP_check_solution(QAP_t *prob, QAP_solution_t *sol){
    int err = 1;
    if (QAP_calc_cst(prob, sol) != sol->cst){
        fprintf (stderr, "Error: Cost of the solution is invalid\n");
        err = -1;
    }
    if (QAP_check_association(prob, sol) == -1){
        fprintf (stderr, "Error: Association poblem\n");
        err = -1;
    }
    return err;
}

void QAP_show_solution(QAP_t *prob, QAP_solution_t *sol){
    int i;
    printf("\nSolution Cust: %d  (%d", sol->cst, sol->perm[0] + 1);
    for (i = 1; i < prob->size; i++){
        printf (" %d", sol->perm[i] + 1);
    }
    printf(")\n\n");
}

