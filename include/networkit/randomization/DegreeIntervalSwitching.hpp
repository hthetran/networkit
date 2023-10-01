// networkit-format
/*
 * EdgeSwitching.hpp
 *
 *  Created on: 29.09.2023
 *      Author:  Manuel Penschuck <networkit@algorithm.engineering>
 */
#ifndef NETWORKIT_RANDOMIZATION_DEGREE_INTERVAL_SWITCHING_HPP_
#define NETWORKIT_RANDOMIZATION_DEGREE_INTERVAL_SWITCHING_HPP_

#include <utility>

#include <networkit/Globals.hpp>
#include <networkit/auxiliary/Random.hpp>
#include <networkit/base/Algorithm.hpp>
#include <networkit/graph/Graph.hpp>

namespace NetworKit {

class DegreeIntervalSwitching : public Algorithm {
public:
    /// Constructs an EdgeSwitch algorithm that contains a COPY of the input graph.
    explicit DegreeIntervalSwitching(const Graph &G,
                                     const std::vector<std::pair<node, node>> &degreeIntervals);

    ~DegreeIntervalSwitching() override = default;

    /**
     * Attempts to carry out numberOfSwitchesPerEdge * numberOfEdges() edge swaps (i.e., rejected
     * swaps are counted as well, see Algorithm's description why)
     * @note This function can be called several times.
     */
    void run() override;

    /// Return a reference to the perturbed graph
    const Graph &getGraph() const { return graph; }

    /**
     * Move graph owned by the algorithm out.
     * @warning Do not call run() after calling moveGraph()
     */
    Graph moveGraph() { return std::move(graph); }

    /// Modify (average) number of switches per edge that will be executed on next run
    void setNumberOfSwitches(uint64_t x) noexcept { numberOfSwitches = x; }

    /// Modify probabilities of switching types; the sum S of non-negative weights must not
    /// exceed 1.0. The remaining probability 1-S correspond to no switch beeing performed.
    void setSwitchingTypeDistribution(double insertDelete, double hingeFlip, double edgeSwitch);

    double getInsertionDeletionProbability() const noexcept { return probInsertionDeletion; }
    double getHingeFlipProbability() const noexcept { return probInsertionDeletion; }
    double getEdgeSwitchProbability() const noexcept { return probEdgeSwitch; }

    uint64_t getNumberOfInsertionsDeletions() const noexcept { return numInsertionsDeletions; }
    uint64_t getNumberOfAttemptedInsertions() const noexcept { return numAttemptedInsertions; }
    uint64_t getNumberOfAttemptedDeletions() const noexcept { return numAttemptedDeletions; }
    uint64_t getNumberOfHingeFlips() const noexcept { return numHingeFlips; }
    uint64_t getNumberOfEdgeSwitches() const noexcept { return numEdgeSwitches; }
    uint64_t getNumberOfLazy() const noexcept { return numLazy; }
    uint64_t getNumberOfSuccessfulInsertionsDeletions() const noexcept {
        return numSuccessfulInsertionsDeletions;
    }
    uint64_t getNumberOfSuccessfulHingeFlips() const noexcept { return numSuccessfulHingeFlips; }
    uint64_t getNumberOfSuccessfulEdgeSwitches() const noexcept {
        return numSuccessfulEdgeSwitches;
    }

    void resetStatistics() {
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

private:
    Graph graph;
    std::vector<std::pair<node, node>> degreeIntervals;
    std::discrete_distribution<node> upperDegreeDistribution;

    uint64_t numberOfSwitches;

    double probInsertionDeletion = 1.0 / 6;
    double probHingeFlip = 1.0 / 6;
    double probEdgeSwitch = 1.0 / 6;

    uint64_t numInsertionsDeletions = 0;
    uint64_t numAttemptedInsertions = 0;
    uint64_t numAttemptedDeletions = 0;
    uint64_t numHingeFlips = 0;
    uint64_t numEdgeSwitches = 0;
    uint64_t numLazy = 0;

    uint64_t numSuccessfulInsertionsDeletions = 0;
    uint64_t numSuccessfulHingeFlips = 0;
    uint64_t numSuccessfulEdgeSwitches = 0;

    node sampleNodeWeightedByDegree(std::mt19937_64 &);
    node sampleRandomNode(std::mt19937_64 &);

    bool tryPerformSingleInsertionDeletion(std::mt19937_64 &gen);
    bool tryPerformSingleHingeFlip(std::mt19937_64 &gen);
    bool tryPerformSingleEdgeSwitch(std::mt19937_64 &gen);

    bool canRemoveNeighborOf(node u) const noexcept {
        return degreeIntervals[static_cast<size_t>(u)].first < graph.degree(u);
    }

    bool canAddNeighborTo(node u) const noexcept {
        return degreeIntervals[static_cast<size_t>(u)].second > graph.degree(u);
    }
};

} // namespace NetworKit

#endif // NETWORKIT_RANDOMIZATION_DEGREE_INTERVAL_SWITCHING_HPP_
