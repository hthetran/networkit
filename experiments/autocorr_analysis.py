#!/usr/bin/env python3
# Created on: Jul 29, 2017
# Script for autocorrelation analysis
#   for Curveball and ESMC
# Generates and analyzes the binary time-series
#   for Powerlaw Degree Sequences

from networkit import *
import copy
import timeit
import argparse
import os
import itertools
import math

path = os.path.dirname(os.path.realpath(__file__))
out_path = "{}/output".format(path)
if not os.path.exists(out_path):
    os.makedirs(out_path)
log_file = "{}/log.out".format(out_path)

class Logger:
    def __init__(self, label, fn):
        self.label = label
        self.fn = fn

    def __enter__(self):
        self.start_time = timeit.default_timer()

    def __exit__(self, a, b, c):
        f.write("%s: Runtime %f s" % (self.label, timeit.default_timer() - self.start_time))

# Arguments
# --nodes       Number of nodes
# --mus         Scaling factor
# --gammas      Degree exponent
# --mindegs     Min degree
# --maxdegs     Max degree
# --runlength   Length of long markov chain run
# --thinnings   Thinning factor of {Z_t}
# --runs        Run size
# -e            Add randomizer ESMC
# -cu           Add randomizer CURVEBALL with uniform trades
# -cg           Add randomizer CURVEBALL with global trades
parser = argparse.ArgumentParser()
parser.add_argument('--nodes', type=int, default=[int(1e5)], nargs="+")
parser.add_argument('--mus', type=float, default=[1.0], nargs="+")
parser.add_argument('--gammas', type=float, default=[-2.0], nargs="+")
parser.add_argument('--mindegs', type=int, default=[50], nargs="+")
parser.add_argument('--maxdegs', type=int, default=[10000], nargs="+")
parser.add_argument('--runlength', type=int, default=1000)
parser.add_argument('--thinnings', type=int, default=[1, 5, 10, 15, 30], nargs="+")
parser.add_argument('--runs', type=int, default=1)
parser.add_argument('-e', dest='rand', action='append_const', const='EMES')
parser.add_argument('-cu', dest='rand', action='append_const', const='CB_UNIFORM')
parser.add_argument('-cg', dest='rand', action='append_const', const='CB_GLOBAL')

args = parser.parse_args()
print("Running configuration:")
print(args)

with open(log_file, 'w') as logf:
    for mu, n, gamma, mindeg, maxdeg, thinning, run in itertools.product(args.mus, args.nodes, args.gammas, args.mindegs, args.maxdegs, args.thinnings, range(args.runs)):
        plds = generators.PowerlawDegreeSequence(math.ceil(mu*mindeg), math.ceil(mu*maxdeg), gamma)
        plds.run()
        hh = generators.HavelHakimiGenerator(plds.getDegreeSequence(math.ceil(mu*n)))
        G = hh.generate()
        for rand in args.rand:
            label = "{}n{}mu{}g{}mindeg{}maxdeg{}runl{}thin{}run{}".format(rand, n, mu, gamma, mindeg, maxdeg, args.runlength, thinning, run)
            print(label)
            if rand == 'EMES':
                for run in range(args.runs):
                    esmc = curveball.EdgeSwitchingMarkovChainRandomization(copy.deepcopy(G))
                    swaps = curveball.UniformTradeGenerator(thinning*G.numberOfEdges(), G.numberOfEdges())
                    aa = curveball.AutocorrelationAnalysis(args.runlength)
                    aa.addSample(G.edges())
                    for chainrun in range(args.runlength - 1):
                        print(chainrun)
                        esmc.run(swaps.generate())
                        aa.addSample(esmc.getEdges()) 
            if rand == 'CB_UNIFORM':
                for run in range(args.runs):
                    cbu = curveball.Curveball(copy.deepcopy(G))
                    swaps = curveball.UniformTradeGenerator(thinning*G.numberOfEdges(), G.numberOfNodes())
                    aa = curveball.AutocorrelationAnalysis(args.runlength)
                    aa.addSample(G.edges())
                    for chainrun in range(args.runlength - 1):
                        print(chainrun)
                        cbu.run(swaps.generate())
                        aa.addSample(esmc.getEdges())
            if rand == 'CB_GLOBAL':
                pass
            
            #TODO: Analyze binary time-series here
            aa.init()
            vec = aa.getTimeSeries()
            print(vec)
            del(aa)
