// networkit-format
#include <stdexcept>

#include <networkit/auxiliary/Random.hpp>
#include <networkit/auxiliary/SignalHandling.hpp>
#include <networkit/randomization/DegreeIntervalSwitchingSeparated.hpp>

namespace NetworKit {

DegreeIntervalSwitchingSeparated::DegreeIntervalSwitchingSeparated(
    const Graph &G, const std::vector<std::pair<node, node>> &degreeIntervals,
    double numberOfSwitchesPerEdge)
    : DegreeIntervalSwitching(G, degreeIntervals, numberOfSwitchesPerEdge) { }

void DegreeIntervalSwitchingSeparated::run() {
    double idRatio = this->DegreeIntervalSwitching::getInsertionDeletionProbability();
    double hfRatio = this->DegreeIntervalSwitching::getHingeFlipProbability();
    double esRatio = this->DegreeIntervalSwitching::getEdgeSwitchProbability();

    count totalNumberOfEdgeSwitches = this->DegreeIntervalSwitching::getNumberOfEdgeSwitches();

    this->DegreeIntervalSwitching::setSwitchingTypeDistribution(1., 0., 0.);
    this->DegreeIntervalSwitching::setNumberOfSwitches(static_cast<count>(static_cast<double>(totalNumberOfEdgeSwitches) * idRatio));
    this->DegreeIntervalSwitching::run();

    this->DegreeIntervalSwitching::setSwitchingTypeDistribution(0., 1., 0.);
    this->DegreeIntervalSwitching::setNumberOfSwitches(static_cast<count>(static_cast<double>(totalNumberOfEdgeSwitches) * hfRatio));
    this->DegreeIntervalSwitching::run();

    this->DegreeIntervalSwitching::setSwitchingTypeDistribution(0., 0., 1.);
    this->DegreeIntervalSwitching::setNumberOfSwitches(static_cast<count>(static_cast<double>(totalNumberOfEdgeSwitches) * esRatio));
    this->DegreeIntervalSwitching::run();

    this->DegreeIntervalSwitching::setSwitchingTypeDistribution(idRatio, hfRatio, esRatio);
    this->DegreeIntervalSwitching::setNumberOfSwitches(totalNumberOfEdgeSwitches);
    hasRun = true;
}

}