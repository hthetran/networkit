#!/usr/bin/env python3
# Date: Jul 26, 2017
# EMES-script for Curveball comparison
# Evaluates graphmetrics
# Uses NetworKit's EMES implementation, therefore analysis directly in python

import os, sys, glob
import tempfile
import subprocess
import socket
import timeit
import getpass
import argparse
import itertools
from networkit import *

parser = argparse.ArgumentParser()
parser.add_argument('--nodes', type=int, default=[int(1e5)], nargs="+")
parser.add_argument('--mus', type=float, default=[1.0], nargs="+")
parser.add_argument('--mindegs', type=int, default=[50], nargs="+")
parser.add_argument('--maxdegs', type=int, default=[10000], nargs="+")
parser.add_argument('--runs', type=int, default=10)
parser.add_argument('-std', default=False, action='store_true')
parser.add_argument('-e', dest='gens', action='append_const', const='EMES')

args = parser.parse_args()
args.measures = []
if args.std:
    args.measures += ['avglcc', 'degass', 'triangle']

print(args)

class Walltime:
    def __init__(self, label):
        self.label = label

    def __enter__(self):
        self.start_time = timeit.default_timer()

    def __exit__(self, a, b, c):
        print("%s Runtime %f s" % (self.label, timeit.default_timer() - self.start_time))

current_directory = os.path.dirname(os.path.abspath(__file__))
print("Current directory: %s" % current_directory)

def genEMES(n=10000, mindeg=50, maxdeg=10000, gamma=-2.0, scaleDeg=1.0, swapsize=10, run=0, outf=subprocess.STDOUT, outFn=None):
    with tempfile.TemporaryDirectory(dir=sysTempDir) as tempdir:
        # file containing a table of metrics per time
        metrics_filename = os.path.join(tempdir, "default")
        if outFn:
            metrics_filename = outFn + ".dat"

# Actual algorithm runs
logdir = "./"
logfile = "edgeswap_stats.csv"
with open(logdir + logfile, 'a') as outf:
    for run, gen, n, mu, minDeg, maxDeg in itertools.product(range(args.runs), args.gens, args.nodes, args.mus, args.mindegs, args.maxdegs):
        print("-" * 100)
        print("Generate. Algo: %s, n: %d, minDeg: %d, maxDeg: %d, scaleDeg: %f" % (gen, n, minDeg, maxDeg, mu))
        label="%s_n%d_kmin%d_kmax%d_mu%.1f-%d" % (gen, n, minDeg, maxDeg, mu, run)
        algoArgs = {'n' : n, 'minDeg' : minDeg, 'maxDeg' : maxDeg, 'scaleDeg' : mu,
                'gamma' : -2, 'outf' : log_outf, 'outFn' : logdir + label,
                'run' : run}
        if gen == "EMES":
            genEMES(**algoArgs)
