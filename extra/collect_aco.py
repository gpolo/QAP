import os
import re
import sys
import subprocess

if len(sys.argv) < 2:
    print "Usage: %s ACOPATH" % sys.argv[0]
    sys.exit(1)

EXECUTIONS = 22
INSTANCES_SYMMETRIC = {
        'chr18b': 1534, 'chr25a': 3796, 'esc32h': 438,
        'esc64a': 116, 'had20': 6922, 'kra30a': 88900, 'kra32': 88700,
        'nug12': 578, 'nug17': 1732, 'nug22': 3596, 'nug30': 6124,
        'rou15': 354210, 'wil50': 48816, 'ste36c': 8239110,
        'tho40': 240516, 'tai60a': 7208572, 'tai25a': 1167256}
INSTANCES_ASYMMETRIC = {'tai50b': 458821517, 'bur26b': 3817852,
        'tai60b': 608215054}

HERE = os.path.abspath(os.path.dirname(os.path.abspath(__file__)))
DATA_DIR = os.path.join(os.path.join(HERE, os.path.pardir), 'data')
ACO = sys.argv[1]


collected_data = []

def do_executions(args):
    print '%s %s' % (sys.argv[0], ' '.join(args))
    for _ in xrange(EXECUTIONS):
        p = subprocess.Popen(args, stdout=subprocess.PIPE)

        data_stdout = p.communicate()[0]
        data = data_stdout.split('\n')

        time_to_best, cost, gen = None, None, None
        for line in data:
            if line.startswith("Best"):
                time_to_best = float(line.split(':')[1].split()[0])
            elif line.startswith("Cost"):
                cost = int(line.split(':')[1])
            elif line.startswith("Generation"):
                gen = int(line.split(':')[1])

        if not all((time_to_best, cost)) or gen is None:
            sys.stderr.write("%s %s %s" % (time_to_best, cost, gen))
            sys.stderr.write("Check your output\n");
            sys.exit(1)

        print "%s\t%d\t%d" % (str(time_to_best).replace('.', ','), cost, gen)


for inst, best in INSTANCES_SYMMETRIC.iteritems():
    n = re.findall(r'\d+', inst)[0]
    args = [ACO, '-s', '-I', '-p', '0.8', '-n', n, '-t', '180',
            '-b', str(best), "../data/%s.dat" % inst] # DATA_DIR XXX
    do_executions(args)
    print

for inst, best in INSTANCES_ASYMMETRIC.iteritems():
    n = re.findall(r'\d+', inst)[0]
    args = [ACO, '-a', '-I', '-p', '0.8', '-n', n, '-t', '180',
            '-b', str(best), "../data/%s.dat" % inst] # DATA_DIR XXX
    do_executions(args)
    print
