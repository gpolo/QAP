import sys
import subprocess

if len(sys.argv) < 2:
    print "Usage: %s all your params" % sys.argv[0]
    sys.exit(1)

EXECUTIONS = 22

for _ in xrange(EXECUTIONS):
    p = subprocess.Popen(sys.argv[1:], stdout=subprocess.PIPE)
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
