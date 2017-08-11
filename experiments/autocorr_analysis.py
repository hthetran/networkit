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
        self.f.write("# %s Runtime %f s\n" % (self.label, timeit.default_timer() - self.start_time))
        print("%s Runtime %f s" % (self.label, timeit.default_timer() - self.start_time))

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
# RUNLENGTH
# GEN
# RAND
# RUN
# THINNING
# MU
# N
# GENPARAM1: GAMMA or P
# GENPARAM2: MINDEG or AVGDEG
# GENPARAM3: MAXDEG or TEMP
# INDEPENDENCERATE
data_line = "{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\n"

params = []
if 'PLDHH' in args.gens:
    L = list(itertools.product(['PLDHH'], args.mus, args.nodes, args.gammas, args.mindegs, args.maxdegs, range(args.runs)))
    params += list(filter(lambda tup : tup[4] <= tup[2] and tup[5] <= tup[2] and tup[4] <= tup[5], L))
if 'ERDOSRENYI' in args.gens:
    L = list(itertools.product(['ERDOSRENYI'], args.mus, args.nodes, args.p, range(args.runs)))
    params += list(filter(lambda tup : tup[2] <= args.ercap, L))
if 'HYPER' in args.gens:
    L = list(itertools.product(['HYPER'], args.mus, args.nodes, args.gammas, args.avgdegs, args.temps, range(args.runs)))
    params += list(filter(lambda tup : tup[4] <= tup[2], L))

print(params)
random.shuffle(params)
params_chunks = chunkify(params, args.pus)

def run(params, pre_fn, args, pid):
    print("{} processes tuple(s) {}.".format(multiprocessing.current_process(), params))
    with open("{}_{}.log".format(pre_fn, pid), 'w') as logf, open("{}_{}.dat".format(pre_fn, pid), 'w') as outf:
        for param in params:
            gen = param[0]
            mu = param[1]
            n = param[2]
            if gen == 'PLDHH':
                gamma = param[3]
                mindeg = param[4]
                maxdeg = param[5]
                run = param[6]
                plds = generators.PowerlawDegreeSequence(math.ceil(mu*mindeg), math.ceil(mu*maxdeg), gamma)
                plds.run()
                hh = generators.HavelHakimiGenerator(plds.getDegreeSequence(math.ceil(mu*n)))
                G = hh.generate()
                label = "{}rand{}mu{}n{}g{}mindeg{}maxdeg{}runl{}run{}".format(gen, "{}", mu, n, gamma, mindeg, maxdeg, args.runlength, run)
            elif gen == 'ERDOSRENYI':
                p = param[3]
                run = param[4]
                G = generators.ErdosRenyiGenerator(math.ceil(mu*n), p).generate()
                label = "{}rand{}mu{}n{}p{}runl{}run{}".format(gen, "{}", mu, n, p, args.runlength, run)
            elif gen == 'HYPER':
                gamma = param[3]
                avgdeg = param[4] 
                temp = param[5]
                run = param[6]
                G = generators.HyperbolicGenerator(math.ceil(mu*n), math.ceil(mu*avgdeg), gamma, temp).generate()
                label = "{}rand{}mu{}n{}g{}avgdeg{}temp{}runl{}run{}".format(gen, "{}", mu, n, gamma, avgdeg, temp, args.runlength, run)
            
            # process the graph
            logf.write("# {}{}\n".format(gen, str(G)))
            for rand in args.rand:
                label = label.format(rand)
                
                small_thinnings = [thinning for thinning in args.thinnings if thinning <= args.thinsplit]
                big_thinnings = [thinning for thinning in args.thinnings if thinning > args.thinsplit] 
                thinnings_partitions = [small_thinnings, big_thinnings]

                # for each thinnings partition
                for part in thinnings_partitions:
                    if len(part) > 0:
                        part_max = max(part)
                        part_gcd = np.ufunc.reduce(gcd, part)
                        part_chainlength = int(part_max * args.runlength / part_gcd)

                        if rand == 'NETWORKIT_ES':
                            randomizer = curveball.EdgeSwitchingMarkovChainRandomization(G)
                            swaps = curveball.UniformTradeGenerator(math.ceil(part_gcd*G.numberOfEdges()/10), G.numberOfEdges())
                        elif rand == 'CB_UNIFORM':
                            randomizer = curveball.Curveball(G)
                            swaps = curveball.UniformTradeGenerator(math.ceil(part_gcd*G.numberOfEdges()/10), G.numberOfNodes())
                        elif rand == 'CB_GLOBAL':
                            #TODO
                            continue
                            
                        with Logger(label, logf):
                            aa = curveball.AutocorrelationAnalysis(part_chainlength + 1)
                            aa.addSample(G.edges())
                            
                            # run long chain
                            for chainrun in range(part_chainlength):
                                randomizer.run(swaps.generate())
                                aa.addSample(randomizer.getEdges()) 
                       
                            # analyze time-series for each small thinning value
                            for thinning in part:
                                logf.write("      {} Thinning: {}\n".format(pid, thinning))
                                last = int(thinning * args.runlength / part_gcd)
                                aa.init()
                                ind_count = 0
                                # Foreach time-series x
                                while True:
                                    end, vec = aa.getTimeSeries()
                                    if end:
                                        break
                                    x = np.zeros((2,2))
                                    get_transitions(vec[0:last:int(thinning/part_gcd)], x)
                                    hat_x = np.zeros((2,2))
                                    get_loglinear_estimate(x, hat_x)
                                    log_sum = sum([x[(i,j)]*math.log(hat_x[(i,j)]/x[(i,j)]) if x[(i,j)] != 0 else 0 for i in range(2) for j in range(2)])
                                    delta_BIC = (-2)*log_sum - math.log(args.runlength - 1)
                                    if (delta_BIC < 0):
                                        ind_count += 1
                                indrate = ind_count/aa.numberOfEdges()

                                # write out to file
                                if gen == 'PLDHH':
                                    outf.write(data_line.format(args.label, args.runlength, gen, rand, run, thinning, mu, n, gamma, mindeg, maxdeg, indrate))
                                elif gen == 'ERDOSRENYI':
                                    outf.write(data_line.format(args.label, args.runlength, gen, rand, run, thinning, mu, n, p, "-", "-", indrate))
                                elif gen == 'HYPER':
                                    outf.write(data_line.format(args.label, args.runlength, gen, rand, run, thinning, mu, n, gamma, avgdeg, temp, indrate))
    print("PU {} finished.".format(pid))

if __name__ == '__main__':
    processes = [multiprocessing.Process(target=run, args=(chunk, pre_fn, args, pid)) for chunk, pid in zip(params_chunks, range(len(params_chunks)))]
    for process in processes:
        process.start()

    for process in processes:
        process.join()
