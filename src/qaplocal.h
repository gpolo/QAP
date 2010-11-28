/*
 * Created:  22/11/2010 09:42:52
 *
 * Author:  Guilherme Polo, ggpolo@gmail.com
 *
 */
#ifndef QAPLOCAL_H
#define QAPLOCAL_H

#include "memetic.h"

typedef int (*ExchangeCostFunc)(int, int, int *, int *, int *, int);

int pairwise_exchange_asymmetric_cost(int, int, int *, int *, int *, int);
int pairwise_exchange_symmetric_cost(int, int, int *, int *, int *, int);
int local_search(struct RecombineInfo *, int *, int *, int *, int, ExchangeCostFunc);

#endif
