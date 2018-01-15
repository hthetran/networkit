#!/usr/bin/env python3

from networkit import *
import pprint
import timeit
import argparse
import csv

parser = argparse.ArgumentParser()
parser.add_argument('--start', type=int, default=4)
parser.add_argument('--end', type=int, default=6)
parser.add_argument('--steps', type=int, default=3)
parser.add_argument('--runlength', type=int, default=10)
parser.add_argument('--runs', type=int, default=25)
parser.add_argument('--maxcount', type=int, default=10**8)
parser.add_argument('--output', type=str, default='data.csv')
parser.add_argument('--const', action='store_true')
args = parser.parse_args()

class Walltime:
    def __init__(self, label):
        self.label = label
    def __enter__(self):
        self.start_time = timeit.default_timer()
    def __exit__(self, a, b, c):
        print("%s Runtime %f s" % (self.label, timeit.default_timer() - self.start_time))

def exp10series(start, end, steps):
    return [int(10**(k/steps + start)) for k in range((end - start) * steps + 1)]

config = []

if args.const:
    const_min_deg = 50
    const_max_deg = 10000
    node_series = exp10series(args.start, args.end, args.steps)
    config.extend(list(zip(node_series,
                      [const_min_deg for _ in range(len(node_series))],
                      [const_max_deg for _ in range(len(node_series))])))

print("Printing Configurations:")
pprint.pprint(config)

with open(args.output, 'a') as out_file:
    writer = csv.writer(out_file, delimiter='\t')
    for (num_nodes, min_deg, max_deg) in config:
        print("[====] At configuration: (%d, %d, %d)" % (num_nodes, min_deg, max_deg))
        pldgen = generators.PowerlawDegreeSequence(min_deg, max_deg, -2)
        pldgen.run()
        avgdeg = pldgen.getExpectedAverageDegree()
        # skip this setting since too much RAM will be used
        if (num_nodes * avgdeg > args.maxcount):
            continue
        for run in range(args.runs):
            print(" Run: %d" % run)
            degseq = pldgen.getDegreeSequence(num_nodes)
            hhgen = generators.HavelHakimiGenerator(degseq)
            G = hhgen.generate()

            start_time = timeit.default_timer()

            algo = curveball.Curveball(G)
            algo.run(curveball.GlobalTradeGenerator(args.runlength, num_nodes).generate())

            end_time = timeit.default_timer()

            print(" Finished in time %f" % (end_time - start_time))
            writer.writerow([num_nodes, min_deg, max_deg, G.numberOfEdges(), end_time - start_time])
