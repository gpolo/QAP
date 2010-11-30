import sys
import math

def get_matrices(n):
    a = []
    b = []
    data = map(int, sys.stdin.read().split())

    for i in xrange(0, len(data)/2, n):
        a.append(data[i:i + n])
    for i in xrange(len(data)/2, len(data), n):
        b.append(data[i:i + n])

    return a, b

def dominance(m):
    mi = (1./(n**2)) * sum(col for row in m for col in row)
    rho2 = (1./(n**2 - 1)) * sum((col - mi)**2 for row in m for col in row)
    rho = math.sqrt(rho2)
    return 100 * (rho/mi)

n = int(raw_input())
a, b = get_matrices(n)
fd, dd = dominance(a), dominance(b)

print "Flow Dominance: %.2f" % fd
print "Distance Dominance: %.2f" % dd
