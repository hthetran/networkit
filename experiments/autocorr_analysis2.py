#!/usr/bin/env python3

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
    def __init__(self, label, fn):
        self.label
        self.fn = fn
    def __enter__(self):
        self.start_time = timeit.default_timer()
        self.fn.write("# {}\n".format(self.label))
        print("{}".format(self.label))
    def __exit__(self, a, b, c):
        self.fn.write("# %s Runtime %f s\n" % (self.label, timeit.default_timer() - self.start_time))
        print("%s Runtime %f s" % (self.label, timeit.default_timer() - self.start_time))

parser = argparse.ArgumentParser()
parser.add_argument('--label', type=str, default="tmp")
parser.add_argument('--pus', type=int, default=4)
parser.add_argument('--nodes', type=int, default=[1000], nargs="+")
parser.add_argument('--a', type=int, default=2)
parser.add_argument('--b', type=int, default=100)
parser.add_argument('--thins', type=int, default=[2, 4, 8, 16, 32], nargs="+")
parser.add_argument('--runs', type=int, default=20)
parser.add_argument('--runlength', type=int, default=500)
parser.add_argument('-e', dest='rand', action='append_const', const='ES', help="Randomize with NetworKit-ES")
parser.add_argument('-cg', dest='rand', action='append_const', const='CBG', help="Randomize with Curveball-IM with uniform trades")

args = parser.parse_args()
print("Running configuration", args)

path = os.path.dirname(os.path.realpath(__file__))
out_path = "{}/output".format(path)
pre_fn = "{}/{}".format(out_path, args.label)

if not os.path.exists(out_path):
    os.makedirs(out_path)

graph_params = [(args.a, args.b, nodes) for nodes in args.nodes]

def get_transitions(series, x):
    for i in range(len(series)-1):
        if series[i+1]
    #TODO

def get_loglinear_estimate(x, hat_x):
    xsum = x[(0,0)] + x[(0,1)] + x[(1,0)] + x[(1,1)]
    hat_x[(0,0)] = (x[(0,0)] + x[(0, 1)])*(x[(0,0)] + x[(1, 0)])/xsum
    hat_x[(0,1)] = (x[(0,0)] + x[(0, 1)])*(x[(0,1)] + x[(1, 1)])/xsum
    hat_x[(1,0)] = (x[(1,0)] + x[(1, 1)])*(x[(0,0)] + x[(1, 0)])/xsum
    hat_x[(1,1)] = (x[(1,0)] + x[(1, 1)])*(x[(0,1)] + x[(1, 1)])/xsum

def run_config(G, pid, rands, rand_steps, rand_method):
    setSeed(random.getrandbits(64), True)
    print("{} subprocess, pid: {}, rand_method: {}".format(multiprocessing.current_process(), pid, rand_method))
    m = G.numberOfEdges()
    n = G.numberOfNodes()
    if rand_method == "ES":
        rand = curveball.EdgeSwitchingMarkovChainRandomization(G)
    elif rand_method == "CBG":
        rand = curveball.Curveball(G)
    with open("{}_{}.dat".format(pre_fn,  pid), 'w') as outf:
        aa = curveball.AutocorrelationAnalysis(len(rand_steps))
        # run long chain
        for step in rand_steps:
            if rand_method == "ES":
                swap = curveball.UniformTradeGenerator(step*m, m)
            elif rand_method == "CBG":
                swap = curveball.GlobalTradeGenerator(step, n)
            rand.run(swap.generate())
            aa.addSample(rand.getEdges())
        m_existed = aa.numberOfEdges()
        print("Percentage of existed edges:", m_existed / (n*(n - 1)/2.0))
        enum = [i for i in range(m_existed)]
        random.shuffle(enum)
        m_sample = math.ceil(m_existed/10)
        enum_sample = enum[0:m_sample]
        enum_sample.sort()
        enum_steps = [enum_sample[0]]
        for i in range(1, len(enum_sample) - 1):
            enum_steps.append(enum_sample[i+1] - enum_sample[i])
        aa.init()
        for aa_step in enum_steps:
            for k in range(aa_step):
                aa.next()
            z = aa.get()
            for thin in args.thins:
                zthin = [z[k-1] for k in range(1,len(rands)) if rands[k-1] % thin == 0]
                x = np.zeros((2,2))
                get_transitions(zthin, x)
                hat_x = np.zeros((2,2))
                get_loglinear_estimate(x, hat_x)
                print(thin, hat_x)

if __name__ == "__main__":
    # generate graphs, since it only makes sense using the same starting graph
    graph_instances = []
    for (a, b, n) in graph_params:
        plds = generators.PowerlawDegreeSequence(a, b, -2)
        plds.run()
        hh = generators.HavelHakimiGenerator(plds.getDegreeSequence(n))
        G = hh.generate()
        graph_instances.append(G)
        graphio.writeGraph(G, "{}/a{}_b{}_n{}.metis".format(out_path, a, b, n), graphio.Format.METIS)
    process_params = list(itertools.product(graph_instances, range(args.runs), args.rand))
    print(process_params)

    # calculate randomization steps
    rands = [0]
    for thin in args.thins:
        for i in range(1, args.runlength + 1):
            rands.append(i * thin)
    rands_set = set(rands)
    rands = list(rands_set)
    rands.sort()
    rand_steps = []
    for j in range(len(rands)-1):
        rand_steps.append(rands[j+1]-rands[j])
    processes = [multiprocessing.Process(target=run_config, args=(G, run_id, rands, rand_steps, rand_method)) for G, run_id, rand_method in process_params]
    for process in processes:
        process.start()
    for process in processes:
        process.join()
