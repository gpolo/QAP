/*
 * Created: Qua 24 Nov 2010 18:21:39 BRST
 *
 * Author: Maycon Sambinelli, msambinelli@gmail.com
 *
 */

#include "QAP.h"
#include "util.h"

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
    if (QAP_check_association(prob, sol) == -1){
        fprintf (stderr, "Error: Association poblem\n");
        return -1;
    }
    if (QAP_calc_cst(prob, sol) != sol->cst){
        fprintf (stderr, "Error: Cost of the solution is invalid\n");
        return -1;
    }
    return 1;
}

void QAP_show_solution(QAP_t *prob, QAP_solution_t *sol){
    printf("Best solution found after: %lf seconds\n", current_user_time_secs());
    printf("Cost: %d\n", sol->cst);
    printf("Generation: %d\n", sol->geration);
}

