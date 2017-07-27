#!/usr/bin/env python3
# Date: Jul 26, 2017
# EMES-script for Autocorrelation analysis
# Generates and analyzes the edge occurrence time series
# Uses NetworKit's EMES implementation, therefore directly in python

import os, sys, glob
import timeit
import argparse
import itertools
from networkit import *

parser = argparse.ArgumentParser()
parser.add_argument('--nodes', type=int, default=[int(1e5)], nargs="+")
parser.add_argument('--mus', type=float, default=[1.0], nargs="+")
parser.add_argument('--mindegs', type=int, default=[50], nargs="+")
parser.add_argument('--maxdegs', type=int, default=[10000], nargs="+")
parser.add_argument('--runsizes', type=int, default=[1, 5, 10, 15], nargs="+")
parser.add_argument('--thinning', type=int, default=[5, 9, 13], nargs="+")
parser.add_argument('-methoda', dest='methods', action='append_const', const='METHODA')
parser.add_argument('-methodb', dest='methods', action='append_const', const='METHODB')
parser.add_argument('-e', dest='randomizers', action='append_const', const='EMES')
parser.add_argument('-c', dest='randomizers', action='append_const', const='CURVEBALL')
parser.add_argument('--runs', type=int, default=10)

args = parser.parse_args()

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

# Actual algorithm run
logdir = "./"
logfile = "edgeswap_autocorr_stats.csv"
with open(logdir + logfile, 'a') as outf:
    for runs, gen, method, runsize, thinning, n, minDeg, maxDeg in itertools.product(range(args.runs), args.gens, args.runsizes, args.thinning, args.nodes, args.mindegs, args.maxdegs):

