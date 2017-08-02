#!/usr/bin/env python3
# Created on: Jul 29, 2017
# Script for autocorrelation analysis
#   for Curveball and ESMC
# Generates and analyzes the binary time-series for
#   Powerlaw Degree Sequences materialized with Havel-Hakimi
#   Erdos-Renyi
#   Hyperbolic
#   Hyperbolic with high degrees
#   Hyperbolic with high temperature

from networkit import *
import copy
import timeit
import argparse
import os
import itertools
import math
import fractions
import numpy as np

class Logger:
    def __init__(self, label, f):
        self.label = label
        self.f = f

    def __enter__(self):
        self.start_time = timeit.default_timer()

    def __exit__(self, a, b, c):
        self.f.write("# %s: Runtime %f s\n" % (self.label, timeit.default_timer() - self.start_time))
        print("%s: Runtime %f s" % (self.label, timeit.default_timer() - self.start_time))

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

def get_loglinear_estimate(x, hat_x):
    hat_x[(0,0)] = (x[(0,0)] + x[(0, 1)])*(x[(0,0)] + x[(1, 0)])/x.sum()
    hat_x[(0,1)] = (x[(0,0)] + x[(0, 1)])*(x[(0,1)] + x[(1, 1)])/x.sum()
    hat_x[(1,0)] = (x[(1,0)] + x[(1, 1)])*(x[(0,0)] + x[(1, 0)])/x.sum()
    hat_x[(1,1)] = (x[(1,0)] + x[(1, 1)])*(x[(0,1)] + x[(1, 1)])/x.sum()

gcd = np.frompyfunc(fractions.gcd, 2, 1)

parser = argparse.ArgumentParser()
parser.add_argument('--label', type=str, default="", help="Label for the output- and logfiles")
parser.add_argument('--nodes', type=int, default=[int(1e5)], nargs="+", help="Number of nodes")
parser.add_argument('--mus', type=float, default=[1.0], nargs="+", help="Scaling factors")
parser.add_argument('--gammas', type=float, default=[-2.0], nargs="+", help="Degree exponents")
parser.add_argument('--mindegs', type=int, default=[50], nargs="+", help="Min degrees")
parser.add_argument('--maxdegs', type=int, default=[10000], nargs="+", help="Max degrees")
parser.add_argument('--runlength', type=int, default=1000, help="Length of time-series")
parser.add_argument('--thinnings', type=int, default=[1, 5, 10, 15, 30], nargs="+", help="Thinning factor for the long chain") #TODO: change to tenths
parser.add_argument('--runs', type=int, default=1, help="Number of randomization per graph")
parser.add_argument('-pldhh', dest='gens', action='append_const', const='PLDHH', help="Generate graphs with Havel-Hakimi from powerlaw degree sequence")
parser.add_argument('-er', dest='gens', action='append_const', const='ERDOSRENYI', help="Generate graphs with Erdos-Renyi")
parser.add_argument('--p', type=float, default=[0.5], nargs="+", help="Probabilities for an edge-existence in Erdos-Renyi")
parser.add_argument('-hyper', dest='gens', action='append_const', const='HYPER_STD', help="Generate graphs with standard Hyperbolic") #TODO find params
parser.add_argument('-hyperhd', dest='gens', action='append_const', const='HYPER_HIGHDEG', help="Generate graphs with high degree Hyperbolic (resembles Exp)") #TODO find params
parser.add_argument('-hyperht', dest='gens', action='append_const', const='HYPER_HIGHTEMP', help="Generate graphs with high temperature Hyperbolic") #TODO find params
parser.add_argument('-e', dest='rand', action='append_const', const='NETWORKIT_ES', help="Randomize with NetworKit-ES")
parser.add_argument('-cu', dest='rand', action='append_const', const='CB_UNIFORM', help="Randomize with Curveball-IM with uniform trades")
parser.add_argument('-cg', dest='rand', action='append_const', const='CB_GLOBAL', help="Randomize with Curveball-IM with global trades") #TODO: not yet implemented

args = parser.parse_args()
print("Running configuration:")
print(args)

if args.gens is None:
    raise RuntimeError("No generators given. Try one of these: -pldhh, -er, -hyper, -hyperhd, -hyperht.")
if args.rand is None:
    raise RuntimeError("No randomization scheme given. Try one of these: -e.")

path = os.path.dirname(os.path.realpath(__file__))
out_path = "{}/output".format(path)
if not os.path.exists(out_path):
    os.makedirs(out_path)
log_fn = "{}/{}.log".format(out_path, args.label)
dat_fn = "{}/{}.dat".format(out_path, args.label)

# Table entries:
# LABEL
# RAND
# MU
# N
# MINDEG
# MAXDEG
# RUN
# THINNING
# RUNLENGTH
# GEN
# ... additional parameters for the generators
# INDRATE
data_line = "{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t" #TODO length may change

with open(log_fn, 'w') as logf, open(dat_fn, 'w') as outf:
    # TODO differentiate between generators better would be on a higher level
    if 'PLDHH' in args.gens:
        for mu, n, gamma, mindeg, maxdeg, run in itertools.product(args.mus, args.nodes, args.gammas, args.mindegs, args.maxdegs, range(args.runs)):
            plds = generators.PowerlawDegreeSequence(math.ceil(mu*mindeg), math.ceil(mu*maxdeg), gamma)
            plds.run()
            hh = generators.HavelHakimiGenerator(plds.getDegreeSequence(math.ceil(mu*n)))
            G = hh.generate()
            logf.write(str(G) + '\n')
            for rand in args.rand:
                # TODO: change randomization scheme
                for thinning in args.thinnings:

                    # initialization depending on randomizer
                    if rand == 'NETWORKIT_ES':
                        randomizer = curveball.EdgeSwitchingMarkovChainRandomization(copy.deepcopy(G))
                        swaps = curveball.UniformTradeGenerator(thinning*G.numberOfEdges(), G.numberOfEdges())
                    elif rand == 'CB_UNIFORM':
                        randomizer = curveball.Curveball(G)
                        swaps = curveball.UniformTradeGenerator(thinning*G.numberOfEdges(), G.numberOfNodes())
                    elif rand == 'CB_GLOBAL':
                        #TODO
                        continue
                    
                    for run in range(args.runs):
                        label = "{}n{}mu{}g{}mindeg{}maxdeg{}runl{}thin{}run{}".format(rand, n, mu, gamma, mindeg, maxdeg, args.runlength, thinning, run)
                        with Logger(label, logf):
                            aa = curveball.AutocorrelationAnalysis(args.runlength + 1)
                            aa.addSample(G.edges())
                            for chainrun in range(args.runlength):
                                randomizer.run(swaps.generate())
                                aa.addSample(randomizer.getEdges()) 
                       
                            aa.init()
                            ind_count = 0
                            # Foreach time-series x
                            while True:
                                end, vec = aa.getTimeSeries()
                                if end:
                                    break
                                x = np.zeros((2,2))
                                get_transitions(vec, x)
                                hat_x = np.zeros((2,2))
                                get_loglinear_estimate(x, hat_x)
                                log_sum = sum([x[(i,j)]*math.log(hat_x[(i,j)]/x[(i,j)]) if x[(i,j)] != 0 else 0 for i in range(2) for j in range(2)])
                                delta_BIC = (-2)*log_sum - math.log(args.runlength)
                                if (delta_BIC < 0):
                                    ind_count += 1
                            # TODO: output into output file
                            print(ind_count/aa.numberOfEdges())
