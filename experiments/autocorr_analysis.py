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
import numpy as np

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

def get_transitions(series, x):
    flat_coords = np.ravel_multi_index((series[:-1], series[1:]), x.shape)
    x.flat = np.bincount(flat_coords, minlength=x.size)

def get_jdd(G):
    f_maxdeg = max([G.degree(u) for u in range(n)])
    jdd = np.zeros((f_maxdeg, f_maxdeg))
    edges = G.edges()
    for u, v in edges:
        jdd[(G.degree(u)-1, G.degree(v)-1)] += 1
        jdd[(G.degree(v)-1, G.degree(u)-1)] += 1
    return jdd

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
parser.add_argument('-help', default=False, action='store_true')

args = parser.parse_args()
print("Running configuration:")
if args.help:
    # TODO
    raise SystemExit(0)
print(args)

with open(log_file, 'w') as logf:
    for mu, n, gamma, mindeg, maxdeg, thinning, run in itertools.product(args.mus, args.nodes, args.gammas, args.mindegs, args.maxdegs, args.thinnings, range(args.runs)):
        plds = generators.PowerlawDegreeSequence(math.ceil(mu*mindeg), math.ceil(mu*maxdeg), gamma)
        plds.run()
        hh = generators.HavelHakimiGenerator(plds.getDegreeSequence(math.ceil(mu*n)))
        G = hh.generate()
        # print graph specs
        # TODO: use logfile
        print(G)
        for rand in args.rand:
            label = "{}n{}mu{}g{}mindeg{}maxdeg{}runl{}thin{}run{}".format(rand, n, mu, gamma, mindeg, maxdeg, args.runlength, thinning, run)
            # TODO: use logfile
            print(label)
            # initialization depending on randomizer
            if rand == 'EMES':
                randomizer = curveball.EdgeSwitchingMarkovChainRandomization(copy.deepcopy(G))
                swaps = curveball.UniformTradeGenerator(thinning*G.numberOfEdges(), G.numberOfEdges())
            elif rand == 'CB_UNIFORM':
                Gcopy = copy.deepcopy(G)
                randomizer = curveball.Curveball(Gcopy)
                swaps = curveball.UniformTradeGenerator(thinning*G.numberOfEdges(), G.numberOfNodes())
            elif rand == 'CB_GLOBAL':
                #TODO
                continue
            
            for run in range(args.runs):
                aa = curveball.AutocorrelationAnalysis(args.runlength + 1)
                aa.addSample(G.edges())
                for chainrun in range(args.runlength):
                    print(chainrun)
                    randomizer.run(swaps.generate())
                    aa.addSample(randomizer.getEdges()) 
            
            aa.init()
            # Foreach time-series x
            count = 0
            while True:
                end, vec = aa.getTimeSeries()
                if end:
                    break
                x = np.zeros((2,2))
                get_transitions(vec, x)
                #x = np.divide(x, args.runlength)
                
                # calculate goodness
                # G^2 = -2*sum(i,j)x_ij*log(hat(x_ij)/x_ij)
                # BIC = G^2 + q*log(K/k - 1) (Here K/k)
                # dBIC = BIC^(I) - BIC^(M)
                #      = -2*sum(i,j)x_ij*log(hat(x_ij^(I))/x_ij) - log(K/k - 1) (Here K/k)
                # where hat(x_ij^(I)) = (x_i+)(x_+j)/(x_++)
                hat_x = np.zeros((2,2))
                hat_x[(0,0)] = (x[(0,0)] + x[(0, 1)])*(x[(0,0)] + x[(1, 0)])/x.sum()
                hat_x[(0,1)] = (x[(0,0)] + x[(0, 1)])*(x[(0,1)] + x[(1, 1)])/x.sum()
                hat_x[(1,0)] = (x[(1,0)] + x[(0, 1)])*(x[(1,0)] + x[(1, 0)])/x.sum()
                hat_x[(1,1)] = (x[(1,0)] + x[(1, 1)])*(x[(0,1)] + x[(1, 1)])/x.sum()
                log_sum = sum([x[(i,j)]*math.log(hat_x[(i,j)]/x[(i,j)]) if x[(i,j)] != 0 else 0 for i in range(2) for j in range(2)])
                delta_BIC = (-2)*log_sum - math.log(args.runlength)
                if (delta_BIC < 0):
                    count += 1
                #print(delta_BIC)
            print(count/G.numberOfEdges())
