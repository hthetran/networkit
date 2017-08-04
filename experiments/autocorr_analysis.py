#!/usr/bin/env python3
# Created on: Jul 29, 2017
# Script for autocorrelation analysis
#   for Curveball and ESMC
# Generates and analyzes the binary time-series for
#   Powerlaw Degree Sequences materialized with Havel-Hakimi
#   Erdos-Renyi
#   Hyperbolic

from networkit import *
import multiprocessing
import copy
import timeit
import argparse
import os
import itertools
import math
import fractions
import random
import numpy as np

class Logger:
    def __init__(self, label, f):
        self.label = label
        self.f = f

    def __enter__(self):
        self.start_time = timeit.default_timer()
        self.f.write("# {}\n".format(self.label))
        print("{}".format(self.label))

    def __exit__(self, a, b, c):
        self.f.write("# Runtime %f s\n" % (timeit.default_timer() - self.start_time))
        print("Runtime %f s" % (timeit.default_timer() - self.start_time))

def chunkify(seq, num):
    avg = len(seq) / float(num)
    out = []
    last = 0.0

    while last < len(seq):
        out.append(seq[int(last):int(last + avg)])
        last += avg

    return out

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
parser.add_argument('--pus', type=int, default="", help="Number of processing units")
parser.add_argument('--nodes', type=int, default=[int(1e5)], nargs="+", help="Number of nodes")
parser.add_argument('--mus', type=float, default=[1.0], nargs="+", help="Scaling factors")
parser.add_argument('--gammas', type=float, default=[-2.0], nargs="+", help="Degree exponents")
parser.add_argument('--mindegs', type=int, default=[50], nargs="+", help="Min degrees")
parser.add_argument('--maxdegs', type=int, default=[10000], nargs="+", help="Max degrees")
parser.add_argument('--runlength', type=int, default=1000, help="Length of time-series")
parser.add_argument('--thinnings', type=int, default=[10, 50, 100, 150, 300], nargs="+", help="Thinning factor for the long chain")
parser.add_argument('--thinsplit', type=int, default=100, help="Thinning values smaller/equal than and values greater processed in bulk")
parser.add_argument('--runs', type=int, default=1, help="Number of randomization per graph")
parser.add_argument('-pldhh', dest='gens', action='append_const', const='PLDHH', help="Generate graphs with Havel-Hakimi from powerlaw degree sequence")
parser.add_argument('-er', dest='gens', action='append_const', const='ERDOSRENYI', help="Generate graphs with Erdos-Renyi")
parser.add_argument('--p', type=float, default=[0.5], nargs="+", help="Probabilities for an edge-existence in Erdos-Renyi")
parser.add_argument('--ercap', type=int, default=100000, help="Cap for nodes on Erdos-Renyi")
parser.add_argument('-hyper', dest='gens', action='append_const', const='HYPER', help="Generate graphs with Hyperbolic")
parser.add_argument('--avgdegs', type=float, default=[6.], nargs="+", help="Avg degrees")
parser.add_argument('--temps', type=float, default=[0.], nargs="+", help="Temperatures")
parser.add_argument('-e', dest='rand', action='append_const', const='NETWORKIT_ES', help="Randomize with NetworKit-ES")
parser.add_argument('-cu', dest='rand', action='append_const', const='CB_UNIFORM', help="Randomize with Curveball-IM with uniform trades")
parser.add_argument('-cg', dest='rand', action='append_const', const='CB_GLOBAL', help="Randomize with Curveball-IM with global trades") #TODO: not yet implemented

args = parser.parse_args()
print("Running configuration:")
print(args)

if args.pus > multiprocessing.cpu_count():
    raise RuntimeError("Not enough PUs in the system, use a number in the range [1..{}].".format(multiprocessing.cpu_count()))
if args.gens is None:
    raise RuntimeError("No generators given. Try one of these: -pldhh, -er, -hyper, -hyperhd, -hyperht.")
if args.rand is None:
    raise RuntimeError("No randomization scheme given. Try one of these: -e.")

path = os.path.dirname(os.path.realpath(__file__))
out_path = "{}/output".format(path)
if not os.path.exists(out_path):
    os.makedirs(out_path)
pre_fn = "{}/{}".format(out_path, args.label)

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
# INDEPENDENCERATE
data_line = "{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t" #TODO length may change

params = list(itertools.product(args.mus, args.nodes, args.gammas, args.mindegs, args.maxdegs, range(args.runs)))
random.shuffle(params)
params_chunks = chunkify(params, args.pus)

def run(params, pre_fn, args, pid):
    print("{} processes tuple(s) {}.".format(multiprocessing.current_process(), params))
    with open("{}_{}.log".format(pre_fn, pid), 'w') as logf, open("{}_{}.dat".format(pre_fn, pid), 'w') as outf:
        for mu, n, gamma, mindeg, maxdeg, run in params:
            for gen in args.gens:
                if gen == 'PLDHH':
                    plds = generators.PowerlawDegreeSequence(math.ceil(mu*mindeg), math.ceil(mu*maxdeg), gamma)
                    plds.run()
                    hh = generators.HavelHakimiGenerator(plds.getDegreeSequence(math.ceil(mu*n)))
                    graphs = [hh.generate()]
                elif gen == 'ERDOSRENYI':
                    if math.ceil(mu*n) <= args.ercap:
                        graphs = [generators.ErdosRenyiGenerator(math.ceil(mu*n), p).generate() for p in args.p]
                    else:
                        graphs = []
                        break
                elif gen == 'HYPER':
                    if abs(gamma) > 2: 
                        graphs = [generators.HyperbolicGenerator(math.ceil(mu*n), math.ceil(mu*avgdeg), abs(gamma), temp).generate() for avgdeg in args.avgdegs for temp in args.temps]
                    else:
                        graphs = []
                        break
                
                # forall graphs generated above
                for G in graphs:
                    logf.write("# {}{}\n".format(gen, str(G)))
                    for rand in args.rand:
                        # TODO: cut hard-coding style by using a list like this [small_thinnings, big_thinnings]
                        small_thinnings = [thinning for thinning in args.thinnings if thinning <= args.thinsplit]
                        
                        if len(small_thinnings) > 0:
                            s_max = max(small_thinnings)
                            s_gcd = np.ufunc.reduce(gcd, small_thinnings)
                            s_chainlength = int(s_max * args.runlength / s_gcd)

                            if rand == 'NETWORKIT_ES':
                                randomizer = curveball.EdgeSwitchingMarkovChainRandomization(G)
                                swaps = curveball.UniformTradeGenerator(math.ceil(s_gcd*G.numberOfEdges()/10), G.numberOfEdges())
                            elif rand == 'CB_UNIFORM':
                                randomizer = curveball.Curveball(G)
                                swaps = curveball.UniformTradeGenerator(math.ceil(s_gcd*G.numberOfEdges()/10), G.numberOfNodes())
                            elif rand == 'CB_GLOBAL':
                                #TODO
                                continue
                                
                            for run in range(args.runs):
                                label = "{}rand{}n{}mu{}g{}mindeg{}maxdeg{}runl{}run{}".format(gen, rand, n, mu, gamma, mindeg, maxdeg, args.runlength, run)
                                with Logger(label, logf):
                                    aa = curveball.AutocorrelationAnalysis(s_chainlength + 1)
                                    aa.addSample(G.edges())
                                    for chainrun in range(s_chainlength):
                                        randomizer.run(swaps.generate())
                                        aa.addSample(randomizer.getEdges()) 
                               
                                    # analyze time-series for each small thinning value
                                    for thinning in small_thinnings:
                                        logf.write("      {} Thinning: {}\n".format(pid, thinning))
                                        last = int(thinning * args.runlength / s_gcd)
                                        aa.init()
                                        ind_count = 0
                                        # Foreach time-series x
                                        while True:
                                            end, vec = aa.getTimeSeries()
                                            if end:
                                                break
                                            x = np.zeros((2,2))
                                            get_transitions(vec[0:last:int(thinning/s_gcd)], x)
                                            hat_x = np.zeros((2,2))
                                            get_loglinear_estimate(x, hat_x)
                                            log_sum = sum([x[(i,j)]*math.log(hat_x[(i,j)]/x[(i,j)]) if x[(i,j)] != 0 else 0 for i in range(2) for j in range(2)])
                                            delta_BIC = (-2)*log_sum - math.log(args.runlength)
                                            if (delta_BIC < 0):
                                                ind_count += 1
                                        # TODO: output into output file
                                        print("{} Ind. rate: {}".format(pid, ind_count/aa.numberOfEdges()))


                        # big thinning values
                        big_thinnings = [thinning for thinning in args.thinnings if thinning > args.thinsplit]
                        if len(big_thinnings) > 0:
                            b_max = max(big_thinnings)
                            b_gcd = np.ufunc.reduce(gcd, big_thinnings)
                            b_chainlength = int(b_max * args.runlength / b_gcd)
 
                            if rand == 'NETWORKIT_ES':
                                randomizer = curveball.EdgeSwitchingMarkovChainRandomization(G)
                                swaps = curveball.UniformTradeGenerator(math.ceil(b_gcd*G.numberOfEdges()/10), G.numberOfEdges())
                            elif rand == 'CB_UNIFORM':
                                randomizer = curveball.Curveball(G)
                                swaps = curveball.UniformTradeGenerator(math.ceil(b_gcd*G.numberOfEdges()/10), G.numberOfNodes())
                            elif rand == 'CB_GLOBAL':
                                #TODO
                                continue
                                
                            for run in range(args.runs):
                                label = "{}rand{}n{}mu{}g{}mindeg{}maxdeg{}runl{}run{}".format(gen, rand, n, mu, gamma, mindeg, maxdeg, args.runlength, run)
                                with Logger(label, logf):
                                    aa = curveball.AutocorrelationAnalysis(b_chainlength + 1)
                                    aa.addSample(G.edges())
                                    for chainrun in range(s_chainlength):
                                        randomizer.run(swaps.generate())
                                        aa.addSample(randomizer.getEdges()) 
                               
                                    # analyze time-series for each small thinning value
                                    for thinning in big_thinnings:
                                        logf.write("      {} Thinning: {}\n".format(pid, thinning))
                                        last = int(thinning * args.runlength / b_gcd)
                                        aa.init()
                                        ind_count = 0
                                        # Foreach time-series x
                                        while True:
                                            end, vec = aa.getTimeSeries()
                                            if end:
                                                break
                                            x = np.zeros((2,2))
                                            get_transitions(vec[0:last:int(thinning/b_gcd)], x)
                                            hat_x = np.zeros((2,2))
                                            get_loglinear_estimate(x, hat_x)
                                            log_sum = sum([x[(i,j)]*math.log(hat_x[(i,j)]/x[(i,j)]) if x[(i,j)] != 0 else 0 for i in range(2) for j in range(2)])
                                            delta_BIC = (-2)*log_sum - math.log(args.runlength)
                                            if (delta_BIC < 0):
                                                ind_count += 1
                                        # TODO: output into output file
                                        print("{} Ind. rate: {}".format(pid, ind_count/aa.numberOfEdges()))

                       
"""
                        for thinning in args.thinnings:
                            #if thinning in small_thinnings:
                            #    step = s_gcd
                            #else:
                            #    step = b_gcd
                            #chain_length = 

                            # initialization depending on randomizer
                            if rand == 'NETWORKIT_ES':
                                randomizer = curveball.EdgeSwitchingMarkovChainRandomization(G)
                                swaps = curveball.UniformTradeGenerator(math.ceil(thinning*G.numberOfEdges()/10), G.numberOfEdges())
                            elif rand == 'CB_UNIFORM':
                                randomizer = curveball.Curveball(G)
                                swaps = curveball.UniformTradeGenerator(math.ceil(thinning*G.numberOfEdges()/10), G.numberOfNodes())
                            elif rand == 'CB_GLOBAL':
                                #TODO
                                continue
                            
                            for run in range(args.runs):
                                label = "{}rand{}n{}mu{}g{}mindeg{}maxdeg{}runl{}thin{}run{}".format(gen, rand, n, mu, gamma, mindeg, maxdeg, args.runlength, thinning, run)
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
"""

if __name__ == '__main__':
    processes = [multiprocessing.Process(target=run, args=(chunk, pre_fn, args, pid)) for chunk, pid in zip(params_chunks, range(len(params_chunks)))]
    for process in processes:
        process.start()

    for process in processes:
        process.join()
