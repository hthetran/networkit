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

// Implement actual switching types (sampling comes later)
bool DegreeIntervalSwitching::tryPerformSingleEdgeSwitchOnNodes(node s1, node t1, node s2,
                                                                node t2) {
    reject_if(s1 == s2);
    reject_if(s1 == t1);
    reject_if(s1 == t2);

    reject_if(s2 == t1);
    reject_if(s2 == t2);
    reject_if(t1 == t2);

    reject_if(!graph.hasEdge(s1, t1));
    reject_if(!graph.hasEdge(s2, t2));

    reject_if(graph.hasEdge(s2, t1));
    reject_if(graph.hasEdge(s1, t2));

    graph.swapEdge(s1, t1, s2, t2);

    accept();
}

bool DegreeIntervalSwitching::tryPerformSingleInsertionDeletionOnNodes(node u, node v) {
    reject_if(u == v);

    if (graph.hasEdge(u, v)) {
        numAttemptedDeletions++;

        // attempt to delete
        reject_if(!canRemoveNeighborOf(u));
        reject_if(!canRemoveNeighborOf(v));

        graph.removeEdge(u, v);
        numSuccessfulDeletions++;

    } else {
        numAttemptedInsertions++;

        // attempt to insert
        reject_if(!canAddNeighborTo(u));
        reject_if(!canAddNeighborTo(v));

        graph.addEdge(u, v);
        numSuccessfulInsertions++;
    }

    accept();
}

bool DegreeIntervalSwitching::tryPerformSingleHingeFlipOnNodes(node u, node v, node w) {
    reject_if(u == v);
    reject_if(u == w);
    reject_if(v == w);

    // remove {u, v} insert {v, w}
    reject_if(!graph.hasEdge(u, v));
    reject_if(graph.hasEdge(v, w));
    reject_if(!canRemoveNeighborOf(u));
    reject_if(!canAddNeighborTo(w));

    graph.removeEdge(u, v);
    graph.addEdge(v, w);

    accept();
}

template <typename ISSample, typename HFSample, typename ESSample>
void DegreeIntervalSwitching::runStrategy(std::mt19937_64 &urng, ISSample iSSampler,
                                          HFSample hFSampler, ESSample eSSampler) {
    Aux::SignalHandler handler;

    const auto probInsertOrHinge = probInsertionDeletion + probHingeFlip;
    const auto probAnySwitch = probInsertOrHinge + probEdgeSwitch;

    for (auto i = 0; i < numberOfSwitches; ++i) {
        handler.assureRunning();

        const auto randomSwitch = std::uniform_real_distribution{}(urng);

        if (randomSwitch < probInsertionDeletion) {
            const bool success = iSSampler(urng);
            numInsertionsDeletions++;
            numSuccessfulInsertionsDeletions += success;
        } else if (randomSwitch < probInsertOrHinge) {
            const bool success = hFSampler(urng);
            numHingeFlips++;
            numSuccessfulHingeFlips += success;
        } else if (randomSwitch < probAnySwitch) {
            const bool success = eSSampler(urng);
            numEdgeSwitches++;
            numSuccessfulEdgeSwitches += success;
        } else {
            numLazy++;
        }
    }
}

void DegreeIntervalSwitching::runStrategySingleSampleEdges(std::mt19937_64 &urng) {
    return runStrategy(
        urng,
        [&](std::mt19937_64 &urng) {
            const node u = sampleRandomNode(urng);
            const node v = sampleRandomNode(urng);

            return tryPerformSingleInsertionDeletionOnNodes(u, v);
        },

        [&](std::mt19937_64 &urng) {
            const node u = sampleNodeWeightedByDegree(urng);
            const node w = sampleRandomNode(urng);

            reject_if(u == w);

            const auto neigh_idx =
                std::uniform_int_distribution<index>{0, graph.degree(u) - 1}(urng);
            const node v = graph.getIthNeighbor(u, neigh_idx);

            return tryPerformSingleHingeFlipOnNodes(u, v, w);
        },

        [&](std::mt19937_64 &urng) {
            const node s1 = sampleNodeWeightedByDegree(urng);
            const node s2 = sampleNodeWeightedByDegree(urng);

            // we avoid GraphTools::randomNeighbor to avoid the implicit cost of accessing the
            // Aux::Random::getURNG
            const auto i1 = std::uniform_int_distribution<index>{0, graph.degree(s1) - 1}(urng);
            const node t1 = graph.getIthNeighbor(s1, i1);

            reject_if(s2 == t1);
            reject_if(graph.hasEdge(s2, t1));

            const auto i2 = std::uniform_int_distribution<index>{0, graph.degree(s2) - 1}(urng);
            const node t2 = graph.getIthNeighbor(s2, i2);

            return tryPerformSingleEdgeSwitchOnNodes(s1, t1, s2, t2);
        });
}

void DegreeIntervalSwitching::runStrategySingleSampleTuples(std::mt19937_64 &urng) {
    return runStrategy(
        urng,
        [&](std::mt19937_64 &urng) {
            const auto u = sampleRandomNode(urng);
            const auto v = sampleRandomNode(urng);
            return tryPerformSingleInsertionDeletionOnNodes(u, v);
        },
        [&](std::mt19937_64 &urng) {
            const auto u = sampleRandomNode(urng);
            const auto v = sampleRandomNode(urng);
            const auto w = sampleRandomNode(urng);
            return tryPerformSingleHingeFlipOnNodes(u, v, w);
        },
        [&](std::mt19937_64 &urng) {
            const auto s1 = sampleRandomNode(urng);
            const auto s2 = sampleRandomNode(urng);
            const auto t1 = sampleRandomNode(urng);
            const auto t2 = sampleRandomNode(urng);
            return tryPerformSingleEdgeSwitchOnNodes(s1, t1, s2, t2);
        });
}

void DegreeIntervalSwitching::runStrategyGlobalSampleTuples(std::mt19937_64 &urng) {
    if (nodes.size() != graph.numberOfNodes()) {
        nodes.clear();
        nodes.reserve(static_cast<size_t>(graph.numberOfNodes()));
        for (node i = 0; i < graph.numberOfNodes(); ++i)
            nodes.push_back(i);
        std::shuffle(nodes.begin(), nodes.end(), urng);
    }

    auto reader = nodes.cbegin();

    auto shuffle_if_below = [&](size_t n) {
        if (std::distance(reader, nodes.cend()) >= n) {
            return;
        }

        std::shuffle(nodes.begin(), nodes.end(), urng);
        reader = nodes.begin();
    };

    return runStrategy(
        urng,
        [&](std::mt19937_64 &urng) {
            shuffle_if_below(2);
            const auto u = *reader++;
            const auto v = *reader++;
            return tryPerformSingleInsertionDeletionOnNodes(u, v);
        },
        [&](std::mt19937_64 &urng) {
            shuffle_if_below(3);
            const auto u = *reader++;
            const auto v = *reader++;
            const auto w = *reader++;
            return tryPerformSingleHingeFlipOnNodes(u, v, w);
        },
        [&](std::mt19937_64 &urng) {
            shuffle_if_below(4);
            const auto s1 = *reader++;
            const auto s2 = *reader++;
            const auto t1 = *reader++;
            const auto t2 = *reader++;
            return tryPerformSingleEdgeSwitchOnNodes(s1, t1, s2, t2);
        });
}

void DegreeIntervalSwitching::run() {
    auto &urng = Aux::Random::getURNG();
    Aux::SignalHandler handler;

    const auto probInsertOrHinge = probInsertionDeletion + probHingeFlip;
    const auto probAnySwitch = probInsertOrHinge + probEdgeSwitch;

    switch (samplingStrategy) {
    case DegreeIntervalSampling::DegreeIntervalSampleSingleEdges:
        runStrategySingleSampleEdges(urng);
        break;

    case DegreeIntervalSampling::DegreeIntervalSampleSingleTuples:
        runStrategySingleSampleTuples(urng);
        break;

    case DegreeIntervalSampling::DegreeIntervalSampleGlobalTuples:
        runStrategyGlobalSampleTuples(urng);
        break;

    default:
        throw std::runtime_error("unknown sampling strategy selected");
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
    const Graph &G, const std::vector<std::pair<node, node>> &degreeIntervals,
    double numberOfSwitchesPerEdge)
    : //
      graph(G), degreeIntervals(degreeIntervals),
      upperDegreeDistribution(
          graph.numberOfNodes(), //
          0.0, static_cast<double>(graph.numberOfNodes()),
          [&](double x) { return degreeIntervals[static_cast<node>(x)].second; }),
      numberOfSwitches(
          static_cast<count>(std::ceil(graph.numberOfEdges() * numberOfSwitchesPerEdge))) {

    if (graph.numberOfNodes() < 4) {
        throw std::runtime_error("Graph needs at least four nodes");
    }

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

void DegreeIntervalSwitching::resetStatistics() {
    numInsertionsDeletions = 0;
    numAttemptedInsertions = 0;
    numAttemptedDeletions = 0;
    numHingeFlips = 0;
    numEdgeSwitches = 0;
    numLazy = 0;
    numSuccessfulInsertionsDeletions = 0;
    numSuccessfulHingeFlips = 0;
    numSuccessfulEdgeSwitches = 0;
}

} // namespace NetworKit
