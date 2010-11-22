/*
 * Created:  22/11/2010 09:36:55
 *
 * Author:  Guilherme Polo, ggpolo@gmail.com
 *
 */

/* Note: Parameters 'a' and 'b' are always treated as a 2-D matrix. */

#include <stdio.h>
#include "qaputil.h"

void
get_matrices(int *a, int *b, int n)
{
    int i, j;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            scanf("%d", &(a[i * n + j]));
        }
    }
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            scanf("%d", &(b[i * n + j]));
        }
    }
}


int
calc_cost(int *p, int *a, int *b, int n)
{
    int i, j;
    int cost = 0;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            cost += a[i * n + j] * b[p[i] * n + p[j]];
        }
    }

    return cost;
}
