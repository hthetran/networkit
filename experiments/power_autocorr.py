from networkit import *
import copy

print("Imported modules")

print("Importing Power")
gPower = graphio.readGraph("power.gml", Format.GML)
for u, v in gPower.edges():
    gPower.addEdge(v, u)

print("Checking autocorrelation of time-series")
print("eps: 4.5e-3 (10*m)")
gTimeSeries = copy.deepcopy(gPower)
swapGen = curveball.UniformTradeGenerator(gPower.numberOfEdges(), gPower.numberOfEdges())
esmc = curveball.EdgeSwitchingMarkovChainRandomization(gTimeSeries)
aa = curveball.AutocorrelationAnalysis(1000)
for i in range(1000):#range(30000*gPower.numberOfEdges()):
    print(i)
    esmc.run(swapGen.generate())
    aa.addSample(esmc.getGraph())

print("Done.")
