import re
import os
import sys
import math

EXECUTIONS = 22

INSTANCE = {
        'chr18b': 1534, 'chr25a': 3796, 'esc32h': 438,
        'esc64a': 116, 'had20': 6922, 'kra30a': 88900, 'kra32': 88700,
        'nug12': 578, 'nug17': 1732, 'nug22': 3596, 'nug30': 6124,
        'rou15': 354210, 'wil50': 48816, 'ste36c': 8239110,
        'tho40': 240516, 'tai60a': 7208572, 'tai25a': 1167256,
        # Asymmetric
        'tai50b': 458821517, 'bur26b': 3817852,
        'tai60b': 608215054}


def extract_data(data, aco=False):
    info = {}

    while True:
        try:
            line = data.next()
        except StopIteration:
            break

        if '.py' not in line:
            raise Exception("Check your input")
        for i in xrange(len(line) - 1, 0, -1):
            if line[i] == '/':
                break

        instance = os.path.splitext(line[i + 1:-1])[0]
        #print ">>", instance, "<< instance"
        info[instance] = {'nbest': 0, 'avg_deviation': 0, 'avg_time_best': 0}
        deviation = 0.0
        for _ in xrange(EXECUTIONS):
            line = data.next()
            time, cost, gen = line.split()
            if int(cost) <= INSTANCE[instance]:
                info[instance]['nbest'] += 1
                info[instance]['avg_time_best'] += float(
                        time.replace(',', '.'))
            else:
                deviation += 100 * ((float(cost) / INSTANCE[instance]) - 1)
        info[instance]['avg_deviation'] = deviation / EXECUTIONS
        if info[instance]['nbest']:
            info[instance]['avg_time_best'] /= info[instance]['nbest']
        else:
            info[instance]['avg_time_best'] = float('NaN')

        try:
            extra = data.next()
        except StopIteration:
            break
        if extra[0] == '#':
            # Discard line
            data.next()

    return info

def comma(s):
    return s.replace('.', ',')

def get_time(info):
    if math.isnan(info['avg_time_best']):
        return '--'
    else:
        return comma('%.2f' % info['avg_time_best'])

def cmp_by_n(a, b):
    n_a = int(re.findall(r'\d+', a)[0])
    n_b = int(re.findall(r'\d+', b)[0])

    if n_a < n_b:
        return -1
    elif n_a > n_b:
        return 1
    elif n_a == n_b:
        return 1 if a > b else -1
    else:
        return 0

def main(am, aco):
    with open(am) as data:
        am_data = extract_data(data)

    with open(aco) as data:
        aco_data = extract_data(data, aco=True)

    #print am_data
    #print '-' * 30
    #print aco_data

    instances = sorted(am_data.keys(), cmp=cmp_by_n)
    for inst in instances:
        am_info = am_data[inst]
        aco_info = aco_data[inst]
        print "%s & %d/22 & %s & %s & %d/22 & %s & %s \\\\" % (
                inst[0].upper() + inst[1:],
                am_info['nbest'], comma("%.4f" % am_info['avg_deviation']),
                get_time(am_info),
                aco_info['nbest'], comma("%.4f" % aco_info['avg_deviation']),
                get_time(aco_info))


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print "Usage: %s AM_DATA ACO_DATA" % sys.argv[0]
        sys.exit(1)

    main(*sys.argv[1:])
