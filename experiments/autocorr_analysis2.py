#!/usr/bin/env python3

from networkit import *
import multiprocessing
import copy
import timeit
import argparse
import getpass
import os
import itertools
import math
import fractions
import random
import glob
import shutil
import csv
import datetime
import tempfile
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
parser.add_argument('--samples', type=int, default=10)
parser.add_argument('--pus', type=int, default=4)
parser.add_argument('--nodes', type=int, default=[1000], nargs="+")
parser.add_argument('--a', type=int, default=2)
parser.add_argument('--b', type=int, default=100)
parser.add_argument('--thins', type=int, default=[8, 16, 32], nargs="+")
parser.add_argument('--runs', type=int, default=20)
parser.add_argument('--runlength', type=int, default=500)
parser.add_argument('-e', dest='rand', action='append_const', const='ES', help="Randomize with NetworKit-ES")
parser.add_argument('-cg', dest='rand', action='append_const', const='CBG', help="Randomize with Curveball-IM with global trades")
parser.add_argument('-cu', dest='rand', action='append_const', const='CBU', help="Randomize with Curveball-IM with uniform trades")

args = parser.parse_args()
print("Running configuration", args)

sysTempDir = tempfile.gettempdir()
if ('SLURM_JOB_ID' in os.environ):
    # presumably we're rnuning on LOEWE CSC
    if not 'NOT_SLURM' in os.environ:
        sysTempDir = '/local/' + str(int(os.environ['SLURM_JOB_ID'])) + '/'
    print("Using SLURM-aware temp. directory: " + sysTempDir)
    path = "/scratch/memhierarchy/%s/independence_%d/proc%d/" % (getpass.getuser(), int(os.environ['SLURM_JOB_ID']), int(os.environ['SLURM_PROCID']))
    if not os.path.exists(path):
          os.mkdir(path)
else:
    path = os.path.dirname(os.path.realpath(__file__))
current_directory = os.path.dirname(os.path.abspath(__file__))
print("current_directory" + current_directory)

out_path = "{}/output".format(path)
pre_fn = "{}/{}".format(out_path, args.label)

if not os.path.exists(out_path):
    os.makedirs(out_path)

graph_params = [(args.a, args.b, nodes) for nodes in args.nodes]

def get_transitions(series, x):
    for i in range(len(series)-1):
        if series[i+1] and series[i]:
            x[(1,1)] += 1
        elif series[i+1] and not series[i]:
            x[(0,1)] += 1
        elif not series[i+1] and series[i]:
            x[(1,0)] += 1
        else:
            x[(0,0)] += 1

def get_loglinear_estimate(x, hat_x):
    xsum = x[(0,0)] + x[(0,1)] + x[(1,0)] + x[(1,1)]
    hat_x[(0,0)] = (x[(0,0)] + x[(0, 1)])*(x[(0,0)] + x[(1, 0)])/xsum
    hat_x[(0,1)] = (x[(0,0)] + x[(0, 1)])*(x[(0,1)] + x[(1, 1)])/xsum
    hat_x[(1,0)] = (x[(1,0)] + x[(1, 1)])*(x[(0,0)] + x[(1, 0)])/xsum
    hat_x[(1,1)] = (x[(1,0)] + x[(1, 1)])*(x[(0,1)] + x[(1, 1)])/xsum

def compute_deltaBIC(x, hat_x):
    xsum = x[(0,0)] + x[(0,1)] + x[(1,0)] + x[(1,1)]
    log_vals = np.zeros((2,2))
    for i, j in itertools.product(range(2), range(2)):
        if x[(i,j)] != 0:
            log_vals[(i,j)] = x[(i,j)]*math.log(hat_x[(i,j)] / x[(i,j)])

    log_sum = log_vals[(0,0)] + log_vals[(0,1)] + log_vals[(1,0)] + log_vals[(1,1)]
    log_term = (-2.0)*log_sum
    return log_term - math.log(xsum)

def run_config(G, sid, pid, rands, rand_steps, rand_method):
    setSeed(random.getrandbits(64), True)
    print("{} subprocess, sid: {}, pid: {}, rand_method: {}".format(multiprocessing.current_process(), sid, pid, rand_method))
    m = G.numberOfEdges()
    n = G.numberOfNodes()
    if rand_method == "ES":
        rand = curveball.EdgeSwitchingMarkovChainRandomization(G)
    elif rand_method == "CBG":
        rand = curveball.Curveball(G)
    elif rand_method == "CBU":
        rand = curveball.Curveball(G)
    with open("{}_{}_{}_{}.dat".format(pre_fn, rand_method, sid, pid), 'a') as outf:
        writer = csv.writer(outf, delimiter='\t')
        aa = curveball.AutocorrelationAnalysis(len(rand_steps))
        # run long chain
        for step in rand_steps:
            if rand_method == "ES":
                # generates pairs of edge-ids, looks misleading!
                swap = curveball.UniformTradeGenerator(step*m, m)
            elif rand_method == "CBG":
                swap = curveball.GlobalTradeGenerator(step, n)
            elif rand_method == "CBU":
                swap = curveball.UniformTradeGenerator(step*n, n)
            rand.run(swap.generate())
            aa.addSample(rand.getEdges())
        m_existed = aa.numberOfEdges()
        enum = [i for i in range(m_existed)]
        random.shuffle(enum)
        m_sample = math.ceil(m_existed/10)
        enum_sample = enum[0:m_sample]
        enum_sample.sort()
        enum_steps = [enum_sample[0]]

        independent_count = dict()
        first_independent = dict()
        chain_length = dict()
        nonexist_thin = dict()
        for thin in args.thins:
            independent_count[thin] = 0
            first_independent[thin] = 0
            chain_length[thin] = 0
            nonexist_thin[thin] = 0
        for i in range(1, len(enum_sample) - 1):
            enum_steps.append(enum_sample[i+1] - enum_sample[i])
        aa.init()
        nonedge_count = 0
        for aa_step in enum_steps:
            for k in range(aa_step):
                aa.next()
            z = aa.get()
            deemed_independent = False
            for thin in args.thins:
                zthin = []
                f_rands = [(ix, k) for ix, k in zip(range(len(rands)),rands) if k % thin == 0]
                for k in range(1, len(f_rands)):
                    if f_rands[k][1] - f_rands[k-1][1] == thin:
                        zthin.append(z[f_rands[k-1][0]])
                    else:
                        break
                chain_length[thin] = len(zthin) - 1
                x = np.zeros((2,2))
                get_transitions(zthin, x)
                if x[(0,0)] == len(zthin)-1:
                    nonexist_thin[thin] += 1
                    nonedge_count += 1
                    continue
                hat_x = np.zeros((2,2))
                get_loglinear_estimate(x, hat_x)
                deltaBIC = compute_deltaBIC(x, hat_x)
                if deltaBIC < 0:
                    if not deemed_independent:
                        first_independent[thin] += 1
                        deemed_independent = True
                    independent_count[thin] += 1
        for thin in args.thins:
            writer.writerow(["IndRate", args.runlength, n, args.a, args.b, m, sid, pid, thin, independent_count[thin]/(m_sample-nonexist_thin[thin]), m_existed / (n*(n-1)/2.0)])
#            writer.writerow(["FstHit", args.runlength, n, args.a, args.b, m, sid, pid, thin, first_independent[thin]/(m_sample-nonexist_thin[thin]), m_existed / (n*(n-1)/2.0)])
        print(nonedge_count)
        return 0

if __name__ == "__main__":
 
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

    # generate graphs, since it only makes sense using the same starting graph
    graph_instances = []
    for (a, b, n) in graph_params:
        plds = generators.PowerlawDegreeSequence(a, b, -2)
        for sample_id in range(args.samples):
            plds.run()
            hh = generators.HavelHakimiGenerator(plds.getDegreeSequence(n))
            G = hh.generate()
            graph_instances.append((G, sample_id))
            #graphio.writeGraph(G, "{}/a{}_b{}_n{}_{}.metis".format(out_path, a, b, n, sample_id), graphio.Format.METIS)
            #for (run_id, rand_method) in itertools.product(range(args.runs), args.rand):
            #    run_config(G, sample_id, run_id, rands, rand_steps, rand_method)
            process_params = [(G, sample_id, run_id, rands, rand_steps, rand_method) for (run_id, rand_method) in itertools.product(range(args.runs), args.rand)]
            processes = [multiprocessing.Process(target=run_config, args=x) for x in process_params]
            print(len(processes))
            batches = math.ceil(len(processes)/float(args.pus))
            for batch in range(batches):
                for process in processes[batch*args.pus:(batch+1)*args.pus]:
                    process.start()
                for process in processes[batch*args.pus:(batch+1)*args.pus]:
                    process.join()
    #process_params = list(itertools.product(graph_instances, range(args.runs), args.rand))
    #print(process_params)

    #processes = [multiprocessing.Process(target=run_config, args=(G, s_id, run_id, rands, rand_steps, rand_method)) for (G, s_id), run_id, rand_method in process_params]
    #for process in processes:
    #    process.start()
    #for process in processes:
    #    process.join()
