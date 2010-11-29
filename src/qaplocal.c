/*
 * Created:  22/11/2010 09:42:49
 *
 * Author:  Guilherme Polo, ggpolo@gmail.com
 *
 */

/* Note: Parameters 'a' and 'b' are always treated as a 2-D matrix. */

#include <string.h>
#include "qaplocal.h"
#include "qaputil.h"
#include "util.h"
#include "memetic.h"

int
pairwise_exchange_asymmetric_cost(int i, int j, int *p, int *a, int *b, int n)
{
    int k;
    int reduction;

    reduction = (a[i * n + i]*(b[p[j] * n + p[j]] - b[p[i] * n + p[i]]) +
            a[i * n + j]*(b[p[j] * n + p[i]] - b[p[i] * n + p[j]]) +
            a[j * n + i]*(b[p[i] * n + p[j]] - b[p[j] * n + p[i]]) +
            a[j * n + j]*(b[p[i] * n + p[i]] - b[p[j] * n + p[j]]));
    for (k = 0; k < n; k++) {
        if (k == i || k == j) continue;
        reduction += (a[k * n + i]*(b[p[k] * n + p[j]] - b[p[k] * n + p[i]]) +
                a[k * n + j]*(b[p[k] * n + p[i]] - b[p[k] * n + p[j]]) +
                a[i * n + k]*(b[p[j] * n + p[k]] - b[p[i] * n + p[k]]) +
                a[j * n + k]*(b[p[i] * n + p[k]] - b[p[j] * n + p[k]]));
    }
    return reduction;
}

int
pairwise_exchange_symmetric_cost(int i, int j, int *p, int *a, int *b, int n)
{
    int k;
    int reduction = 0;

    for (k = 0; k < n; k++) {
        if (k == i || k == j) continue;
        reduction += ((a[j * n + k] - a[i * n + k]) *
                (b[p[i] * n + p[k]] - b[p[j] * n + p[k]]));
    }
    reduction *= 2;
    return reduction;
}


int
local_search(struct RecombineInfo *rec, int *p, int *a, int *b, int n,
        ExchangeCostFunc exchange_cost)
{
    int best_cost = calc_cost(p, a, b, n);
    int best_reduction, reduction;
    int swap[2] = {-1, -1};

    int i, j;

    while (1) {
        best_reduction = 0;

#if 0
        /* Algorithm L. Specific for t = 2 */
        int j;
        int combination[4] = {0, 1, n, 0};
        int perm[2];
        while (1) {
            perm[0] = combination[0];
            perm[1] = combination[1];

            /* Check the cost of this pairwise interchange. */
            reduction = exchange_cost(perm[0], perm[1], p, a, b, n);
            if (reduction < best_reduction) { 
                best_reduction = reduction;
                memcpy(swap, perm, sizeof(swap));
            } else if (reduction == best_reduction && RANDOM_UNIT() < 0.1) {
                best_reduction = reduction;
                memcpy(swap, perm, sizeof(swap));
            }
            /* End of cost check. */

            j = 0;
            while (combination[j] + 1 == combination[j + 1]) {
                combination[j] = j;
                j++;
            }
            if (j == 2)
                break;
            combination[j]++;
        }
#endif

        for (i = rec->start; i < n; i++) {
            for (j = i + 1; j < n; j++) {
                reduction = exchange_cost(rec->index[i], rec->index[j],
                        p, a, b, n);
                if (reduction < best_reduction) {
                    best_reduction = reduction;
                    swap[0] = rec->index[i];
                    swap[1] = rec->index[j];
                }
            }
        }

        if (best_reduction < 0) {
            int temp;
            temp = p[swap[0]];
            p[swap[0]] = p[swap[1]];
            p[swap[1]] = temp;
            best_cost += best_reduction;
        } else {
            break;
        }
    }

    return best_cost;
}
