#!/usr/bin/env python3
# Created on: Jul 29, 2017
# Script for autocorrelation analysis
#   for Curveball and ESMC
# Generates and analyzes the binary time-series for
#   Powerlaw Degree Sequences materialized with Havel-Hakimi
#   Erdos-Renyi
#   Hyperbolic
# Outputs metrics
#   Independence-Rate
#   Local-Clustering-Coefficient
#   Degree Assortativitiy
#   Diameter

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
import glob
import shutil
import datetime
import numpy as np

random.seed()

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
parser.add_argument('--pus', type=int, default=1, help="Number of processing units")
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
parser.add_argument('--repeats', type=int, default=1, help="Number of repeats for a graph")
parser.add_argument('--repeatpus', type=int, default=1, help="Number of PUs for repeat runs")
parser.add_argument('-cg', dest='rand', action='append_const', const='CB_GLOBAL', help="Randomize with Curveball-IM with global trades")
parser.add_argument('-ind', dest='metrics', action='append_const', const='IND', help="Analyze Independence-Rate")
parser.add_argument('-lcc', dest='metrics', action='append_const', const='LCC', help="Analyze Local Clustering Coefficient")
parser.add_argument('-da', dest='metrics', action='append_const', const='DA', help="Analyze Degree-Assortativity")
parser.add_argument('-diam', dest='metrics', action='append_const', const='DIAM', help='Analyze Diameter')
parser.add_argument('-ji', dest='metrics', action='append_const', const='JI', help="Analyze Jaccard-Index") #TODO
parser.add_argument('-tri', dest='metrics', action='append_const', const='TRI', help="Analyze Triangle count")

args = parser.parse_args()
print("Running configuration:")
print(args)

if args.pus > multiprocessing.cpu_count():
    raise RuntimeError("Not enough PUs in the system, use a number in the range [1..{}].".format(multiprocessing.cpu_count()))
if args.gens is None:
    raise RuntimeError("No generators given. Try one of these: -pldhh, -er, -hyper.")
if args.rand is None:
    raise RuntimeError("No randomization scheme given. Try one of these: -e.")
if args.metrics is None:
    raise RuntimeError("No metrics given. Try one of these: -ind, -lcc, -da, -degass")

if ("DIAM" in args.metrics) or ("LCC" in args.metrics) or ("DA" in args.metrics):
    graphNeeded = True
else:
    graphNeeded = False

path = os.path.dirname(os.path.realpath(__file__))
out_path = "{}/output".format(path)
pre_fn = "{}/{}".format(out_path, args.label)

if len(glob.glob("{}_*".format(pre_fn))) > 0:
    now = datetime.datetime.now()
    args.label = str(now).replace(' ', '_')
    path = os.path.dirname(os.path.realpath(__file__))
    out_path = "{}/output".format(path)
    pre_fn = "{}/{}".format(out_path, args.label)
    print("Label already used, instead using {}.".format(args.label))

if not os.path.exists(out_path):
    os.makedirs(out_path)

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
# DATETYPE: INDEPENDENCERATE, LCC, DEGASS, ...
# VALUE
data_line = "{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\n"

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
    with open("{}_{}.log".format(pre_fn, pid), 'w') as logf:
        for param in params:
            gen = param[0]
            mu = param[1]
            n = param[2]
            if gen == 'PLDHH':
                gamma = param[3]
                mindeg = param[4]
                maxdeg = param[5]
                run = param[6]
                param_dict = {"gamma" : gamma, "mindeg" : mindeg, "maxdeg" : maxdeg, "run" : run, "gen" : gen, "mu" : mu, "n" : n}
                plds = generators.PowerlawDegreeSequence(math.ceil(mu*mindeg), math.ceil(mu*maxdeg), gamma)
                plds.run()
                hh = generators.HavelHakimiGenerator(plds.getDegreeSequence(math.ceil(mu*n)))
                G = hh.generate()
                label = "{}rand{}mu{}n{}g{}mindeg{}maxdeg{}runl{}run{}".format(gen, "{}", mu, n, gamma, mindeg, maxdeg, args.runlength, run)
            elif gen == 'ERDOSRENYI':
                p = param[3]
                run = param[4]
                param_dict = {"p" : p, "run" : run, "gen" : gen, "mu" : mu, "n" : n}
                G = generators.ErdosRenyiGenerator(math.ceil(mu*n), p).generate()
                label = "{}rand{}mu{}n{}p{}runl{}run{}".format(gen, "{}", mu, n, p, args.runlength, run)
            elif gen == 'HYPER':
                gamma = param[3]
                avgdeg = param[4]
                temp = param[5]
                run = param[6]
                param_dict = {"gamma" : gamma, "avgdeg" : avgdeg, "temp" : temp, "run" : run, "gen" : gen, "mu" : mu, "n" : n}
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
                            degrees = [G.degree(i) for i in range(G.numberOfNodes())]
                            avg_deg = sum(degrees)/float(len(degrees))
                            swaps = curveball.UniformTradeGenerator(math.ceil(part_gcd*G.numberOfEdges()/(10*avg_deg)), G.numberOfNodes())
                        elif rand == 'CB_GLOBAL':
                            randomizer = curveball.Curveball(G)
                            swaps = curveball.GlobalTradeGenerator(math.ceil(part_gcd/10), G.numberOfNodes())

                        with Logger(label, logf):
                            repeat_processes = [multiprocessing.Process(target=chainrun, args=(G, swaps, randomizer, rand, args, param_dict, part, part_gcd, part_chainlength, logf, pre_fn, pid, rpids)) for rpids in chunkify(range(args.repeats), args.repeatpus)]

                            for process in repeat_processes:
                                process.start()
                            for process in repeat_processes:
                                process.join()

    print("PU {} finished.".format(pid))


def chainrun(G, swaps, randomizer, rand, args, param_dict, part, part_gcd, part_chainlength, logf, prefn, pid, rpids):
    print(G)
    setSeed(random.getrandbits(64), True)
    print("{} subprocess, rand: {}.".format(multiprocessing.current_process(), rand))

    # preformat output-line depending on thinning, metric and value
    mu = param_dict["mu"]
    n = param_dict["n"]
    gen = param_dict["gen"]
    run = param_dict["run"]
    if param_dict["gen"] == 'PLDHH':
        gamma = param_dict["gamma"]
        mindeg = param_dict["mindeg"]
        maxdeg = param_dict["maxdeg"]
        out_line = data_line.format(args.label, args.runlength, gen, rand, run, "{}", mu, n, gamma, mindeg, maxdeg, "{}", "{}")
    elif param_dict["gen"] == 'ERDOSRENYI':
        p = param_dict["p"]
        out_line = data_line.format(args.label, args.runlength, gen, rand, run, "{}", mu, n, p, "-", "-", "{}", "{}")
    elif param_dict["gen"] == 'HYPER':
        gamma = param_dict["gamma"]
        avgdeg = param_dict["avgdeg"]
        temp = param_dict["temp"]
        out_line = data_line.format(args.label, args.runlength, gen, rand, run, "{}", mu, n, gamma, avgdeg, temp, "{}", "{}")

    for rpid in rpids:
        with open("{}_{}_{}.dat".format(pre_fn, pid, rpid), 'a') as outf:
            if "IND" in args.metrics:
                aa = curveball.AutocorrelationAnalysis(part_chainlength)

            # run long chain
            randsteps = 0
            parttmp = copy.deepcopy(part)
            for chainrun in range(part_chainlength):
                randomizer.run(swaps.generate())
                randsteps += part_gcd
                if graphNeeded:
                    G = randomizer.getGraph()
                    for thinning in parttmp:
                        if randsteps % thinning == 0:
                            if "TRI" in args.metrics:
                                G.indexEdges()
                                outf.write(out_line.format(thinning, "TRI", sum(sparsification.TriangleEdgeScore(G).run().scores()) // 3))
                            if "DIAM" in args.metrics:
                                outf.write(out_line.format(thinning, "DIAM", distance.Diameter(G, distance.DiameterAlgo.Exact).run().getDiameter()[0]))
                            if "LCC" in args.metrics:
                                lccs = centrality.LocalClusteringCoefficient(G, True).run().scores()
                                avg_lcc = sum(lccs)/float(len(lccs))
                                outf.write(out_line.format(thinning, "LCC", avg_lcc))
                            if "DA" in args.metrics:
                                degrees = centrality.DegreeCentrality(copy.deepcopy(G)).run().scores()
                                outf.write(out_line.format(thinning, "DA", correlation.Assortativity(G, degrees).run().getCoefficient()))

                        if randsteps/thinning > args.runlength:
                            parttmp.remove(thinning)

                if "IND" in args.metrics:
                    aa.addSample(randomizer.getEdges())

            if "IND" in args.metrics:
                indrates = aa.getIndependenceRate(part, args.runlength)
                for thinning, indrate in zip(part, indrates):
                    print(indrate)
                    outf.write(out_line.format(thinning, "IND", indrate))

if __name__ == '__main__':
    processes = [multiprocessing.Process(target=run, args=(chunk, pre_fn, args, pid)) for chunk, pid in zip(params_chunks, range(len(params_chunks)))]
    for process in processes:
        process.start()

    for process in processes:
        process.join()

    with open("{}.dat".format(pre_fn), 'wb') as ffile:
        for sfn in glob.glob("{}_*".format(pre_fn)):
            with open(sfn, 'rb') as subfile:
                shutil.copyfileobj(subfile, ffile)
