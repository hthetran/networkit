// networkit-format
#include <stdexcept>

#include <networkit/auxiliary/Random.hpp>
#include <networkit/auxiliary/SignalHandling.hpp>
#include <networkit/randomization/DegreeIntervalSwitching.hpp>

#define reject_if(X) \
    if (X) \
        return false;
#define accept() return true

namespace NetworKit {

bool DegreeIntervalSwitching::tryPerformSingleEdgeSwitch(std::mt19937_64 &urng) {
    const auto s1 = sampleNodeWeightedByDegree(urng);
    const auto s2 = sampleNodeWeightedByDegree(urng);

    // we avoid GraphTools::randomNeighbor to avoid the implicit cost of accessing the
    // Aux::Random::getURNG
    const auto i1 = std::uniform_int_distribution<index>{0, graph.degree(s1) - 1}(urng);
    const auto t1 = graph.getIthNeighbor(s1, i1);

    reject_if(s2 == t1);
    reject_if(graph.hasEdge(s2, t1));

    const auto i2 = std::uniform_int_distribution<index>{0, graph.degree(s2) - 1}(urng);
    const auto t2 = graph.getIthNeighbor(s2, i2);

    reject_if(t1 == t2);
    reject_if(s1 == t2);
    reject_if(graph.hasEdge(s1, t2));

    graph.swapEdge(s1, t1, s2, t2);

    accept();
}

bool DegreeIntervalSwitching::tryPerformSingleInsertionDeletion(std::mt19937_64 &urng) {
    const auto u = sampleRandomNode(urng);
    const auto v = sampleRandomNode(urng);

    reject_if(u == v);

    if (graph.hasEdge(u, v)) {
        numAttemptedDeletions++;

        // attempt to delete
        reject_if(!canRemoveNeighborOf(u));
        reject_if(!canRemoveNeighborOf(v));

        graph.removeEdge(u, v);

    } else {
        numAttemptedInsertions++;

        // attempt to insert
        reject_if(!canAddNeighborTo(u));
        reject_if(!canAddNeighborTo(v));

        graph.addEdge(u, v);
    }

    accept();
}

bool DegreeIntervalSwitching::tryPerformSingleHingeFlip(std::mt19937_64 &urng) {
    const auto u = sampleNodeWeightedByDegree(urng);
    const auto w = sampleRandomNode(urng);

    reject_if(u == w);

    const auto neigh_idx = std::uniform_int_distribution<index>{0, graph.degree(u) - 1}(urng);
    const auto v = graph.getIthNeighbor(u, neigh_idx);

    reject_if(v == w);

    // remove {u, v} insert {v, w}
    reject_if(!canRemoveNeighborOf(u));
    reject_if(!canAddNeighborTo(w));

    graph.removeEdge(u, v);
    graph.addEdge(v, w);

    accept();
}

void DegreeIntervalSwitching::run() {
    auto &urng = Aux::Random::getURNG();
    Aux::SignalHandler handler;

    const auto probInsertOrHinge = probInsertionDeletion + probHingeFlip;
    const auto probAnySwitch = probInsertOrHinge + probEdgeSwitch;

    while (numberOfSwitches--) {
        handler.assureRunning();

        const auto randomSwitch = std::uniform_real_distribution{}(urng);

        if (randomSwitch < probInsertionDeletion) {
            const bool success = tryPerformSingleInsertionDeletion(urng);
            numInsertionsDeletions++;
            numSuccessfulInsertionsDeletions += success;
        } else if (randomSwitch < probInsertOrHinge) {
            const bool success = tryPerformSingleHingeFlip(urng);
            numHingeFlips++;
            numSuccessfulHingeFlips += success;
        } else if (randomSwitch < probAnySwitch) {
            const bool success = tryPerformSingleEdgeSwitch(urng);
            numEdgeSwitches++;
            numSuccessfulEdgeSwitches += success;
        } else {
            numLazy++;
        }
    }

    hasRun = true;
}

void DegreeIntervalSwitching::setSwitchingTypeDistribution(double insertDelete, double hingeFlip,
                                                           double edgeSwitch) {
    if (insertDelete < 0.0 || hingeFlip < 0.0 || edgeSwitch < 0.0)
        throw std::invalid_argument("Probability may not be negative");

    if (insertDelete + hingeFlip + edgeSwitch > 1.0)
        throw std::invalid_argument("Cumulative swap probabilities must not exceed 1.0");

    probInsertionDeletion = insertDelete;
    probHingeFlip = hingeFlip;
    probEdgeSwitch = edgeSwitch;
}

DegreeIntervalSwitching::DegreeIntervalSwitching(
    const Graph &G, const std::vector<std::pair<node, node>> &degreeIntervals)
    : graph(G), degreeIntervals(degreeIntervals),
      upperDegreeDistribution(graph.numberOfNodes(), //
                              0.0, static_cast<double>(graph.numberOfNodes()), [&](double x) {
                                  return degreeIntervals[static_cast<node>(x)].second;
                              }) {
    if (static_cast<size_t>(graph.numberOfNodes()) != degreeIntervals.size())
        throw std::invalid_argument("Size of degreeIntervals has to match number of nodes");

    for (node u = 0; u < G.numberOfNodes(); ++u) {
        const auto [lb, ub] = degreeIntervals[static_cast<size_t>(u)];

        if (lb > ub)
            throw std::invalid_argument(
                "Lower degree bounds have to be smaller or equal than the upper degree bounds");

        auto const deg = G.degree(u);

        if (lb > deg || ub < deg)
            throw std::invalid_argument("Provided graph does not match degree interval");
    }
}

node DegreeIntervalSwitching::sampleNodeWeightedByDegree(std::mt19937_64 &gen) {
    while (true) {
        const node candidate = upperDegreeDistribution(gen);

        const auto degree = graph.degree(candidate);
        const auto upper = degreeIntervals[static_cast<size_t>(candidate)].second;

        if (degree == upper)
            return candidate;

        // rejection sampling to correct for actual degree
        if (degree < std::uniform_int_distribution<node>{0, upper}(gen))
            return candidate;
    }
}

node DegreeIntervalSwitching::sampleRandomNode(std::mt19937_64 &gen) {
    return std::uniform_int_distribution<index>{0, graph.numberOfNodes() - 1}(gen);
}

} // namespace NetworKit
