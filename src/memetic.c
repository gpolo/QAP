/*
 * Created:  15/11/2010 09:22:42
 *
 * Author:  Guilherme Polo, ggpolo@gmail.com
 *
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "memetic.h"
#include "qaputil.h"
#include "qaplocal.h"
#include "util.h"
#include "flags.h"

struct Individual population[POPSIZE];
static int pop_initialized = 0;
static int n = -1; /* Instance size. */


static void
recombine(int *p1, int *p2, int *offspring, int n, struct RecombineInfo *result)
{
    int i, k;
    double j;
    int elem, to_fill[n], used[n], p1_map[n], p2_map[n], index;

    memset(result->index, 1, sizeof(to_fill));

    memset(to_fill, 1, sizeof(to_fill));
    memset(used, 0, sizeof(used));
    for (i = 0; i < n; i++) {
        p1_map[p1[i]] = i;
        p2_map[p2[i]] = i;
    }

    for (i = 0; i < n; i++) {
        if (!to_fill[i]) {
            continue;
        } else if (p1[i] == p2[i]) {
            offspring[i] = p1[i];
            to_fill[i] = 0;
            result->index[i] = 0; /* Fixed position. */
            continue;
        }

        index = i;
        to_fill[index] = 0;
        j = RANDOM_UNIT();
        if (j < 0.5) {
            /* Pick from first parent. */
            //printf(" P1 %d\n", i);
            offspring[index] = p1[index];
            elem = p2[index];
            while (!used[elem]) {
                used[elem] = 1;
                index = p1_map[elem];
                elem = p2[index];
                offspring[index] = p1[index];
                to_fill[index] = 0;
            }
        } else {
            //printf(" P2 %d\n", i);
            offspring[index] = p2[index];
            elem = p1[index];
            while (!used[elem]) {
                used[elem] = 1;
                index = p2_map[elem];
                elem = p1[index];
                offspring[index] = p2[index];
                to_fill[index] = 0;
            }
        }
    }

    /* Adjust index. */
    for (k = i = n - 1; i >= 0; i--) {
        if (result->index[i]) {
            result->index[k--] = i;
        }
    }
    k++;
    result->start = k;
}

static void
mutate(int *individual, int threshold, int n)
{
    int orig[n];
    int desired_dist = abs(n - lround(threshold / 5.));
    int achieved_dist = 0;
    int f1, f2, temp;

    memcpy(orig, individual, n * sizeof(int));

    int used[n], i, j;
    memset(used, 0, sizeof(used));
    /* XXX Maybe do this "random picking" in a better way
     * (efficiently and random-wise). */
    f1 = RANDOM(n);
    while (achieved_dist != desired_dist) {
        f2 = RANDOM(n);
        if (used[f2]) {
            for (i = 0; i < n - 1; i++) {
                j = (f2 + 1) % n;
                if (!used[j]) {
                    f2 = j;
                    break;
                }
            }
        }
        used[f2] = 1;

        //printf("%d <-> %d  [%d, %d]\n", f1, f2, achieved_dist, desired_dist);
        temp = individual[f1];
        individual[f1] = individual[f2];
        individual[f2] = temp;
        f1 = f2;
        achieved_dist = distance(individual, orig, n);
    }
}


static int
is_equal(struct Individual *ia, struct Individual *ib, int n)
{
    int i;

    if (ia->cost != ib->cost) {
        return 0;
    }
    /* Check if any of the elements differ (two solutions may have
     * the same cost with different elements). */
    for (i = 0; i < n; i++) {
        if (ia->data[i] != ib->data[i]) {
            /* They differ. */
            return 0;
        }
    }
    return 1;
}

static int
sorted_insert(struct Individual *pop, struct Individual *new,
        int *curr_size, int n)
{
    int i, j;
    int elem = new->cost;
    int low = 0, high = *curr_size, mid;

    if (elem >= pop[high - 1].cost) {
        if (high < POPSIZE) {
            pop[high].cost = elem;
            for (i = 0; i < n; i++) pop[high].data[i] = new->data[i];
            (*curr_size)++;
            return 1;
        }
        return 0;
    }

    while (low < high) {
        mid = ((high - low) / 2) + low;
        if (elem < pop[mid].cost) {
            high = mid;
        } else {
            low = mid + 1;
        }
    }

    if (low == 0 || !is_equal(&(pop[low - 1]), new, n)) {
        /* Move everyone after low one place to the right and
         * place this new element on pop[low]. */
        high = *curr_size - 1;
        if (*curr_size < POPSIZE) {
            /* Add a new element. */
            high++;
            (*curr_size)++;
        }
        for (i = high; i > low; i--) {
            pop[i].cost = pop[i - 1].cost;
            for (j = 0; j < n; j++)
                pop[i].data[j] = pop[i -1].data[j];
        }

        pop[low].cost = elem;
        for (i = 0; i < n; i++) pop[low].data[i] = new->data[i];
        return 1;
    }
    return 0;
}

static int
individual_qsort_cmp(const void *a, const void *b)
{
    const struct Individual *ia = a;
    const struct Individual *ib = b;

    if (ia->cost < ib->cost) {
        return -1;
    } else if (ia->cost == ib->cost) {
        return 0;
    }
    return 1;
}

static void
initial_pop(struct Individual *population, int *a, int *b, int n,
        ExchangeCostFunc ecf, int start, struct RecombineInfo *recstd)
{
    int i, j, cost;
    int p[n], r[n];

    for (i = 0; i < n; i++)
        p[i] = i;

    for (i = start; i < INITIAL_POPSIZE; i++) {
        memcpy(r, p, sizeof(r));
        random_shuffle(r, n);
        cost = local_search(recstd, r, a, b, n, ecf);
        population[i].cost = cost;
        for (j = 0; j < n; j++) population[i].data[j] = r[j];
    }
    qsort(population, INITIAL_POPSIZE, sizeof(struct Individual),
            individual_qsort_cmp);
}

int
cond_limit_time(double used, double limit, int gen)
{
    if (used < limit) {
        return 1;
    }
    return 0;
}

int
cond_limit_generations(double used, double limit, int gen)
{
    if (gen < GENERATIONS) {
        return 1;
    }
    return 0;
}

int
memetic(struct ParseResult *pinfo)
{
    int asymmetric = pinfo->flags & ASYMMETRIC;
    int verbose = pinfo->flags & VERBOSE;
    int use_time_limit = pinfo->flags & TIME_LIMIT;
    int stop_with_cost = ((pinfo->flags & ARTIFICIAL) ?
            pinfo->artificial_cost_limit : INT_MIN);
    double time_limit = pinfo->time_limit;

    CondFunc cond;

    /* Instance data. */
    Scanf(1, "%d", &n);
    int a[n][n], b[n][n];

    int i, j, k;
    struct RecombineInfo recresult, recstd;

    recresult.index = Malloc(n * sizeof(int));
    recstd.index = Malloc(n * sizeof(int));
    for (i = 0; i < n; i++) {
        recstd.index[i] = i;
    }
    recstd.start = 0;

    ExchangeCostFunc exchange_cost;

    if (asymmetric) {
        exchange_cost = pairwise_exchange_asymmetric_cost;
    } else {
        exchange_cost = pairwise_exchange_symmetric_cost;
    }

    get_matrices(MATRIX2D(a), MATRIX2D(b), n);

    /* Create initial population. */
    int popsize = INITIAL_POPSIZE;
    struct Individual population[POPSIZE];

    for (i = 0; i < POPSIZE; i++) {
        population[i].data = Malloc(n * sizeof(int));
    }

    int generation = 0;//GENERATIONS;

    double start_time;

    start_time = current_usertime_secs();
    /* XXX Calculate time to initialize population too. */
    initial_pop(population, MATRIX2D(a), MATRIX2D(b), n, exchange_cost, 0,
            &recstd);
    if (population[0].cost <= stop_with_cost) {
        /* Already hit the expected value. */
        time_limit = 0;
        generation = GENERATIONS;
    }
    pop_initialized = 1;

    struct Individual p1, p2, offspring;
    int popchanged, gens_nochange, k1, k2;
    int threshold = (n / 2) + (n / 7);

    if (verbose) {
        printf("Threshold for mutation: %d\n", threshold);
        printf("Population size = %d\n", popsize);
    }

    offspring.data = Malloc(n * sizeof(int));

    double time_to_best;
    int best = population[0].cost, gen_best = 0;
    int gens_without_best_improve = 0;

    gens_nochange = 0;
    time_to_best = 0;

    if (use_time_limit) {
        cond = cond_limit_time;
        if (verbose) {
            printf("Time limit: %f\n", time_limit);
        }
    } else {
        cond = cond_limit_generations;
    }


    while (cond(current_usertime_secs() - start_time, time_limit, generation)) {
        generation++;

        popchanged = 0;
        for (i = 0; i < RECOMBINATIONS; i++) {
            /* Random sample */
            k1 = (int)(RANDOM_UNIT() * popsize);
            k2 = (int)(RANDOM_UNIT() * popsize);
            if (k2 == k1) {
                if (k2 > 0) {
                    k2--;
                } else {
                    k2++;
                }
            }
            p1 = population[k1];
            p2 = population[k2];

            recombine(p1.data, p2.data, offspring.data, n, &recresult);
            /* Check if the offspring is different from either p1 or p2. */
            int p1diff = 0, p2diff = 0;
            for (j = 0; j < n; j++) {
                if (offspring.data[j] != p1.data[j]) {
                    p1diff = 1;
                }
                if (offspring.data[j] != p2.data[j]) {
                    p2diff = 1;
                }
                if (p1diff & p2diff)
                    break;
            }
            if (!(p1diff & p2diff))
                continue;

            /* XXX To remove the test, change recresult to recstd. */
            offspring.cost = local_search(&recresult, offspring.data,
                    MATRIX2D(a), MATRIX2D(b), n, exchange_cost);
            /* Insert in the population only if it differs from all the other
             * individuals. */
            popchanged = sorted_insert(population, &offspring, &popsize, n);
        }

        if (!popchanged) {
            gens_nochange++;
        } else {
            gens_nochange = 0;
        }

        /* Checking for convergence. */
        float cases = (popsize * (popsize - 1)) / 2;
        int total_dist = 0;
        for (i = 0; i < popsize; i++) {
            for (j = i + 1; j < popsize; j++) {
                total_dist += distance(population[i].data, population[j].data,
                        n);
            }
        }
        if (verbose) {
            printf("%d %f, %d\n", total_dist, total_dist / cases, popsize);
        }

        if (gens_nochange == MUTATE_XGENS_STAGNATE ||
                (total_dist / cases) < threshold) {
            /* Mutate all but the best and the worst */
            if (verbose) { 
                printf("Mutation!\n");
            } else {
                printf("M");
            }

            for (i = 1; i < popsize - 1; i++) {
                mutate(population[i].data, threshold, n);
                population[i].cost = local_search(&recstd, population[i].data,
                        MATRIX2D(a), MATRIX2D(b), n, exchange_cost);
            }
            /* Reorder population. */
            qsort(population, popsize, sizeof(struct Individual),
                    individual_qsort_cmp);

            /* Eliminate duplicates. */
            j = 0;
            for (i = 1; i < popsize; i++) {
                if (!is_equal(&(population[i]), &(population[j]), n)) {
                    j++;
                    population[j].cost = population[i].cost;
                    for (k = 0; k < n; k++) {
                        population[j].data[k] = population[i].data[k];
                    }
                }
            }
            popsize = ++j;
        }

        if (verbose) {
            printf("End of generation %d, population size: %d, "
                    "best cost: %d -- %d\n",
                    generation, popsize, population[0].cost,
                    gens_without_best_improve);
            show_sol(population[0].data, n, population[0].cost);
            printf("\n");
        } else {
            printf(".");
            fflush(stdout);
        }


#if DEBUG
        if (best < population[0].cost) {
            /* This shouldn't happen if everything is right. */
            abort();
        }
#endif
        

        if (best == population[0].cost) {
            gens_without_best_improve++;
            if (gens_without_best_improve == 500) {
                if (verbose) {
                    printf("Recreating initial population (keep only the best)!\n");
                } else {
                    printf("R");
                }
                initial_pop(population, MATRIX2D(a), MATRIX2D(b), n,
                        exchange_cost, 1, &recstd);
                popsize = INITIAL_POPSIZE;
                gens_without_best_improve = 0;
            }
        } else {
            time_to_best = current_usertime_secs() - start_time;
            best = population[0].cost;
            gen_best = generation;
            gens_without_best_improve = 0;
        }

        /* Artificial option. */
        if (best <= stop_with_cost) {
            break;
        }
    }

    if (!verbose) {
        printf("\n");
    }

    printf("Best solution found after: %f seconds\n",
            ((time_to_best > 0) ? time_to_best :
             current_usertime_secs() - start_time));
    printf("Cost: %d\nGeneration: %d\n", best, gen_best);

    for (i = 0; i < POPSIZE; i++) {
        free(population[i].data);
    }
    free(offspring.data);
    free(recresult.index);
    free(recstd.index);

    return 0;
}
