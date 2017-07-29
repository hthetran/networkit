#!/usr/bin/env python3

from networkit import *
import copy
import timeit
import os

path = os.path.dirname(os.path.realpath(__file__))
f = open(path + "/out.log", 'w')

class Walltime:
	def __init__(self, label, f):
		self.label = label

	def __enter__(self):
		self.start_time = timeit.default_timer()

	def __exit__(self, a, b, c):
		f.write("%s: Runtime %f s" % (self.label, timeit.default_timer() - self.start_time))

gPower = graphio.readGraph("power.gml", Format.GML)
for u, v in gPower.edges():
	gPower.addEdge(v, u)

gTimeSeries = copy.deepcopy(gPower)
swapGen = curveball.UniformTradeGenerator(gPower.numberOfEdges(), gPower.numberOfEdges())
esmc = curveball.EdgeSwitchingMarkovChainRandomization(gTimeSeries)
aa = curveball.AutocorrelationAnalysis(1000)
with Walltime("Power", f):
	for i in range(1000):#range(30000*gPower.numberOfEdges()):
		esmc.run(swapGen.generate())
		aa.addSample(esmc.getEdges())

f.close()
