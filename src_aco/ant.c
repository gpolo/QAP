/*
 * Created: Qua 24 Nov 2010 16:44:48 BRST
 *
 * Author: Maycon Sambinelli, msambinelli@gmail.com
 *
 */

#include <float.h>
#include <math.h>
#include <assert.h>

#include "ant.h"
#include "util.h"

int found_artificial_value = 0;

QAP_solution_t **ant;
QAP_solution_t *best_solution;

double pheromone[N][N];
float avg;
float prob_best = 0.005; 

float phero_max(float pers){
    return (1.0 / (1 - pers )) * (1.0 / best_solution->cst);
}

float phero_min(float pers, int size){
    float min = phero_max(pers) / (avg - 1) * (pow(prob_best, 1.0 / size) - 1);
    return (min > phero_max(pers))? phero_max(pers) : min;
}

int next_location(set_t *s){
    int i, e;
    i = RANDOM(s->size); 
    e = s->data[i];
    s->data[i] = s->data[s->size - 1];
    s->size--;
    return e; 
}

/* lct: location 
 * flt: facility */
int next_facility(QAP_t *p, set_t *s, int lct){

    /*
    int fcl;
    int x = rand() % s->size;
    fcl = s->data[x];
    s->data[x] = s->data[s->size - 1];
    s->size--;
    return fcl;
    */

    double delta, x, prob[N], last;
    int flt, i;

    delta = 0;
    for (i = 0; i < p->size; i++) {
        delta += pheromone[lct][i];       
    }

    for (i = 0; i < s->size; i++) {
        flt = s->data[i];
        prob[i] = pheromone[lct][flt] / delta;       
    }

    x = (double) RANDOM_UNIT() / INT_MAX;

    last = 0;
    for (i = 0; i < s->size; i++) {
        if (x <= last + prob[i]) {
            flt = s->data[i];
            break;
        }
        last += prob[i];
    }

    s->data[i] = s->data[s->size - 1];
    s->size--;

    return flt;
}

void set_init(set_t *s, int n){
    int i;
    s->size = n;       
    for (i = 0; i < n; i++) s->data[i] = i;
}

QAP_solution_t *ant_build_solution(QAP_t *p){

    int i, j, lct, flt;
    set_t set_lct, set_flt; 
    QAP_solution_t *sol;

    sol = Malloc(sizeof(QAP_solution_t));

    set_init(&set_lct, p->size);
    set_init(&set_flt, p->size);

    /* flt: facility */
    for (i = 0; i < p->size; i++){
        lct = next_location(&set_lct);       
        flt = next_facility(p, &set_flt, lct);
        sol->perm[lct] = flt;
    }

    sol->cst = 0;
    for (i = 0; i < p->size; i++){
        for (j = i;  j < p->size; j++){
            sol->cst += p->dist[i][j] * p->flow[sol->perm[i]][sol->perm[j]];
            if (i != j){
                sol->cst += p->dist[j][i] * p->flow[sol->perm[j]][sol->perm[i]];
            }
        }
    }

    return sol;
}

void ant_system_init(QAP_t *prob){
    int i, j;

    ant = Malloc(prob->nants * sizeof(QAP_solution_t*));

    avg = prob->size / 2.0;

    best_solution =  Malloc(sizeof(QAP_solution_t));
    best_solution->cst = INT_MAX;

    for (i = 0; i < prob->size; i++)
        for (j = 0; j < prob->size; j++)
            pheromone[i][j] = FLT_MAX;  // não eh um erro inicializar com FLT_MAX
}

void ant_construct_solutions(QAP_t *prob){
    int i;
    for (i = 0; i < prob->nants; i++){
        ant[i] = ant_build_solution(prob); 
    }
}

int ite_best_ant(QAP_t *prob){
    int best = 0, i; /* iteration best */
    int best_cst = ant[0]->cst;

    for (i = 1; i < prob->nants; i++){
        if (ant[i]->cst < best_cst){
            best_cst = ant[i]->cst;
            best = i;
        }
    }
    return best; 
}

void aco_update_pheromones(QAP_t *prob){
    int l, f, i;
    QAP_solution_t *best_ant;

    if (prob->flags & PHEUP_IB){
        best_ant = ant[ite_best_ant(prob)]; 
    } else { 
        best_ant = best_solution;
    }

    assert(best_ant != NULL);

    for (l = 0; l < prob->size; l++){
        for (f = 0; f < prob->size; f++){
            if (best_ant->perm[l] == f)
                pheromone[l][f] = prob->phero_pers * pheromone[l][f] + (1.0 / best_ant->cst);
            else 
                pheromone[l][f] = prob->phero_pers * pheromone[l][f]; 


            if (pheromone[l][f] > phero_max(prob->phero_pers))
                pheromone[l][f] = phero_max(prob->phero_pers);
            else {
                if (pheromone[l][f] < phero_min(prob->phero_pers, prob->size))
                    pheromone[l][f] = phero_min(prob->phero_pers, prob->size);
            }
        }
    }

    for (i = 0; i < prob->nants; i++){
        free(ant[i]);
    }
}

int symmetric_calc_reduction(QAP_t *prob, QAP_solution_t *sol, int r, int s){
    int k;
    int reduction = 0;
    for (k = 0; k < prob->size; k++){
         if (k == r || k == s) continue;   
         reduction += (prob->dist[k][r] - prob->dist[k][s]) * (FLOW(prob, sol, k, s) - FLOW(prob, sol, k, r));

    }
    return reduction * 2;
}

int asymmetric_calc_reduction(QAP_t *prob, QAP_solution_t *sol, int r, int s){
    int k;
    int reduction =  prob->dist[r][r] * (FLOW(prob, sol, s, s) - FLOW(prob, sol, r, r)) + 
                     prob->dist[r][s] * (FLOW(prob, sol, s, r) - FLOW(prob, sol, r, s)) +
                     prob->dist[s][r] * (FLOW(prob, sol, r, s) - FLOW(prob, sol, s, r)) + 
                     prob->dist[s][s] * (FLOW(prob, sol, r, r) - FLOW(prob, sol, s, s));

    for (k = 0; k < prob->size; k++){
         if (k == r || k == s) continue;   
         reduction += prob->dist[k][r] * (FLOW(prob, sol, k, s) - FLOW(prob, sol, k, r)) +
                      prob->dist[k][s] * (FLOW(prob, sol, k, r) - FLOW(prob, sol, k, s)) + 
                      prob->dist[r][k] * (FLOW(prob, sol, s, k) - FLOW(prob, sol, r, k)) +
                      prob->dist[s][k] * (FLOW(prob, sol, r, k) - FLOW(prob, sol, s, k));

    }
    return reduction;
}

void two_opt(QAP_t *prob, QAP_solution_t *sol, calc_reduction_exchange calc_reduction){
    int max_gain, gain, i, j, i_max, j_max, aux;
    while (1){
        max_gain = 0;
        for (i = 0; i < prob->size; i++){
            for (j = i + 1; j < prob->size; j++){
                gain = calc_reduction (prob, sol, i, j);    /* obs: gain na verdade é o custo da reduçao */ 
                if (gain < max_gain){
                    max_gain = gain;
                    i_max = i;
                    j_max = j;
                }
            }
        }

        if (max_gain == 0) break;

        if (prob->flags & VERBOSE){
            printf("2OPT -- Reduction %d  Change Facility: %d %d\n", max_gain, sol->perm[i_max], sol->perm[j_max]);
        }

        aux = sol->perm[i_max];
        sol->perm[i_max] = sol->perm[j_max];
        sol->perm[j_max] = aux;
        sol->cst += max_gain;

        //if ((prob->flags & BESTSTOP) && (sol->cst == prob->best_know_solution)) break;
        if ((prob->flags & BESTSTOP) && (sol->cst == prob->best_know_solution)) found_artificial_value = 1;
    }
}

void aco_local_search(QAP_t *prob){
    int i = 0;
    calc_reduction_exchange function = (prob->flags & SYMMETRIC)? symmetric_calc_reduction : asymmetric_calc_reduction;

    for (i = 0; i < prob->nants; i++){
        two_opt(prob, ant[i], function); 
        if (ant[i]->cst < best_solution->cst){
            *best_solution = *ant[i]; 
            best_solution->time = current_user_time_secs();
            //if ((prob->flags & BESTSTOP) && (best_solution->cst == prob->best_know_solution)) break;
            if ((prob->flags & BESTSTOP) && (best_solution->cst == prob->best_know_solution)) found_artificial_value = 1;
        }
    }
}

int stop_condition(QAP_t *prob, int geration){
    if (found_artificial_value) return 1;
    if ((prob->flags & MGENERATION) && prob->ngenerations <= geration) return 1;
    if ((prob->flags & TIMESTOP) && prob->time_limit <= current_user_time_secs()) return 1;
    if ((prob->flags & BESTSTOP) && best_solution->cst <= prob->best_know_solution) return 1;
    return 0;
}

QAP_solution_t *aco(QAP_t *prob){
    int i;
    QAP_solution_t *sol;
    ant_system_init(prob);
    for (i = 0; !stop_condition(prob, i); i++) {
        ant_construct_solutions(prob);
        aco_local_search(prob);
        aco_update_pheromones(prob);

        if (prob->flags & VERBOSE){
            printf("\nGeneration %d -- Best Cost %d\n\n", i, best_solution->cst);
        } else {
            printf(".");    
            fflush(stdout);
        }
    }

    if (found_artificial_value)
        best_solution->time = current_user_time_secs();

    //QAP_check_solution(prob, best_solution);

    sol = Malloc(sizeof(QAP_solution_t));
    *sol = *best_solution;
    sol->geration = i;
    return sol;
}
