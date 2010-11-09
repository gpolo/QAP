import sys
import random

def get_matrix(n):
    while n:
        line = raw_input().split()
        if not line: continue
        n -= 1
        yield map(int, line)

n = int(raw_input())
p = range(n)
a = list(get_matrix(n))
b = list(get_matrix(n))

best = None
min_cost = sys.maxint
no_improvements = 0
limit_noimprov = 4242

while no_improvements != limit_noimprov:
    cost = sum(a[i][j] * b[p[i]][p[j]] for i in xrange(n) for j in xrange(n))

    no_improvements += 1
    if cost < min_cost:
        min_cost = cost
        best = p[:]
        no_improvements = 0

    random.shuffle(p)

print best, min_cost
