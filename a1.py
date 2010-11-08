import sys
import random
import time

seed = hash(time.time()) # Or your favourite one
random.seed(seed)

stdin = sys.stdin.next

def get_matrix(n):
    l = []

    while n:
        line = stdin().split()
        if not line:
            # Empty line, discard
            continue
        n -= 1
        l.append(map(int, line))

    return l

def show_matrix(m):
    for row in m:
        print '  '.join(map(str, row))


n = int(stdin())
p = range(n)
a = get_matrix(n)
b = get_matrix(n)

#print "Problem size: %d" % n
#print
#show_matrix(a)
#print
#show_matrix(b)
#print

min_cost = sys.maxint
no_improvements = 0
limit_noimprov = 1000

while 1:
    cost = 0
    for i in xrange(n):
            for j in xrange(n):
                cost += a[i][j] * b[p[i]][p[j]]

    if cost < min_cost:
        min_cost = cost
        print p, cost
        no_improvements = 0
    else:
        no_improvements += 1

    if no_improvements == limit_noimprov:
        print "No improvements in %d iterations, stopping.." % limit_noimprov
        break

    random.shuffle(p)

print "Seed: %d" % seed
