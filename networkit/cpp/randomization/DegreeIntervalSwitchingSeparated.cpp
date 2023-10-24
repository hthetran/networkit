// networkit-format
#include <stdexcept>

#include <networkit/auxiliary/Random.hpp>
#include <networkit/auxiliary/SignalHandling.hpp>
#include <networkit/randomization/DegreeIntervalSwitchingSeparated.hpp>

namespace NetworKit {

DegreeIntervalSwitchingSeparated::DegreeIntervalSwitchingSeparated(
    const Graph &G, const std::vector<std::pair<node, node>> &degreeIntervals,
    double numberOfSwitchesPerEdge)
    : DegreeIntervalSwitching(G, degreeIntervals, numberOfSwitchesPerEdge),
      numberOfInitialEdges(G.numberOfEdges())
{ }

void DegreeIntervalSwitchingSeparated::run() {
    double idRatio = this->DegreeIntervalSwitching::getInsertionDeletionProbability();
    double hfRatio = this->DegreeIntervalSwitching::getHingeFlipProbability();
    double esRatio = this->DegreeIntervalSwitching::getEdgeSwitchProbability();

    count totalNumberOfEdgeSwitches = this->DegreeIntervalSwitching::getNumberOfSwitches();
    count iterations = totalNumberOfEdgeSwitches / numberOfInitialEdges;
    count id_switches = static_cast<count>(static_cast<double>(numberOfInitialEdges) * idRatio);
    count hf_switches = static_cast<count>(static_cast<double>(numberOfInitialEdges) * hfRatio);
    count es_switches = static_cast<count>(static_cast<double>(numberOfInitialEdges) * esRatio);

    for (count i = 0; i < iterations; ++i) {
        this->DegreeIntervalSwitching::setSwitchingTypeDistribution(1., 0., 0.);
        this->DegreeIntervalSwitching::setNumberOfSwitches(id_switches);
        this->DegreeIntervalSwitching::run();

        this->DegreeIntervalSwitching::setSwitchingTypeDistribution(0., 1., 0.);
        this->DegreeIntervalSwitching::setNumberOfSwitches(hf_switches);
        this->DegreeIntervalSwitching::run();

        this->DegreeIntervalSwitching::setSwitchingTypeDistribution(0., 0., 1.);
        this->DegreeIntervalSwitching::setNumberOfSwitches(es_switches);
        this->DegreeIntervalSwitching::run();
    }

    this->DegreeIntervalSwitching::setSwitchingTypeDistribution(idRatio, hfRatio, esRatio);
    this->DegreeIntervalSwitching::setNumberOfSwitches(totalNumberOfEdgeSwitches);
    hasRun = true;
}

}