#!/usr/bin/env python3

from networkit import *
import pprint
import timeit
import argparse
import csv
import sys
import tempfile
import numpy as np
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument('--start', type=int, default=4)
parser.add_argument('--end', type=int, default=6)
parser.add_argument('--steps', type=int, default=3)
parser.add_argument('--runlength', type=int, default=10)
parser.add_argument('--runs', type=int, default=25)
parser.add_argument('--maxcount', type=int, default=10**8)
parser.add_argument('--trades', type=int, default=5)
parser.add_argument('--output', type=str, default='data.csv')
parser.add_argument('--const', action='store_true')
parser.add_argument('--linear', action='store_true')
parser.add_argument('--with_vles', type=str, default='')

parser.add_argument('--with_boost', action='store_true')
parser.add_argument('--without_boost', action='store_true')

args = parser.parse_args()

boosts = []
if not (args.without_boost or args.with_boost or args.with_vles):
    print("Select at least one algorithm --with_boost / --without_boost")
    sys.exit(-1)
else:
    if (args.without_boost):
        boosts.append(False)
    if (args.with_boost):
        boosts.append(True)

if not (args.linear or args.const):
    print("Select at least one scaling --linear / --const")
    sys.exit(-1)


def exp10series(start, end, steps):
    return [int(10**(k/steps + start)) for k in range((end - start) * steps + 1)]

config = []

if args.const:
    const_min_deg = 50
    const_max_deg = 10000
    node_series = exp10series(args.start, args.end, args.steps)
    config.extend(list(zip(node_series,
                      [const_min_deg] * len(node_series),
                      [const_max_deg] * len(node_series),
                      ["const"] * len(node_series))))

if args.linear:
    const_min_deg = 10
    node_series = exp10series(args.start, args.end, args.steps)
    config.extend(list(zip(node_series,
                           [const_min_deg] * len(node_series),
                           [x // 20 for x in node_series],
                           ["linear"] * len(node_series))))


print("Printing Configurations:")
pprint.pprint(config)

with open(args.output, 'a') as out_file:
    writer = csv.writer(out_file, delimiter='\t')
    for run in range(args.runs):
        for (num_nodes, min_deg, max_deg, scale) in config:
            print("[====] At configuration: (%d, %d, %d, %s)" % (num_nodes, min_deg, max_deg, scale))
            pldgen = generators.PowerlawDegreeSequence(min_deg, max_deg, -2)
            pldgen.run()
            avgdeg = pldgen.getExpectedAverageDegree()
            # skip this setting since too much RAM will be used
            if (num_nodes * avgdeg > args.maxcount):
                continue

            degseq = pldgen.getDegreeSequence(num_nodes)
            hhgen = generators.HavelHakimiGenerator(degseq)
            G = hhgen.generate()

            print("[    ] Graph has %d edges" % G.numberOfEdges())

            list_trades = [curveball.GlobalTradeGenerator(args.runlength, num_nodes).generate() for _ in range(args.trades)]

            for boost in boosts:
                print("[ ===] Boost: ", boost)
                algo = curveball.Curveball(G, boost)
                for r in range(args.trades):
                    start_time = timeit.default_timer()
                    algo.run(list_trades[r])
                    end_time = timeit.default_timer()
                    print("[  ==] Finished round %d in time %f" % (r, end_time - start_time))

                    writer.writerow([scale, "emcb" if boost else "imcb", r, num_nodes, min_deg, max_deg, G.numberOfEdges(), end_time - start_time])
                    out_file.flush()

                del(algo)

            del(list_trades)

            if args.with_vles:
                with tempfile.TemporaryDirectory() as dir:
                    outf = dir + "/dist"
                    dd_raw = np.array(centrality.DegreeCentrality(G).run().scores(), dtype=int)
                    dd = np.vstack(np.unique(dd_raw, return_counts=True)).T
                    np.savetxt(outf, dd, fmt="%d %d")
                    with open("/dev/null", "w") as fnull:
                        p = subprocess.Popen([args.with_vles, "-v", "-t", "-d", outf], stdout=fnull, stderr=subprocess.PIPE)
                    _, err = p.communicate()
                    last_lines = str(err).split("\\n")[-3:-1]

                    assert("Performed : " in last_lines[0])
                    assert("Time used: " in last_lines[1])

                    performed = int(last_lines[0].split(" ")[-2])
                    time = float(last_lines[1].split(" ")[-1])

                    expef = args.trades * G.numberOfEdges() * 2
                    print("Performed %d swaps (%d expected) in %f s" % (performed, expef, time))
                    assert(performed == expef)

                    writer.writerow([scale, "vles", -1 * args.trades, num_nodes, min_deg, max_deg, G.numberOfEdges(), time])
