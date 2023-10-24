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

// the fields have a lengthy name since we need to treat this
// class enum as an enum in Python ...
enum class DegreeIntervalSampling : int {
    DegreeIntervalSampleSingleEdges,
    DegreeIntervalSampleSingleTuples,
    DegreeIntervalSampleGlobalTuples,
};

class DegreeIntervalSwitching : public Algorithm {
public:
    /// Constructs an EdgeSwitch algorithm that contains a COPY of the input graph.
    explicit DegreeIntervalSwitching(const Graph &G,
                                     const std::vector<std::pair<node, node>> &degreeIntervals,
                                     double numberOfSwitchesPerEdge = 10.0);

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
    void setNumberOfSwitches(count x) noexcept { numberOfSwitches = x; }

    /// Modify probabilities of switching types; the sum S of non-negative weights must not
    /// exceed 1.0. The remaining probability 1-S correspond to no switch beeing performed.
    void setSwitchingTypeDistribution(double insertDelete, double hingeFlip, double edgeSwitch);

    double getInsertionDeletionProbability() const noexcept { return probInsertionDeletion; }
    double getHingeFlipProbability() const noexcept { return probInsertionDeletion; }
    double getEdgeSwitchProbability() const noexcept { return probEdgeSwitch; }

    count getNumberOfInsertionsDeletions() const noexcept { return numInsertionsDeletions; }
    count getNumberOfAttemptedInsertions() const noexcept { return numAttemptedInsertions; }
    count getNumberOfAttemptedDeletions() const noexcept { return numAttemptedDeletions; }
    count getNumberOfHingeFlips() const noexcept { return numHingeFlips; }
    count getNumberOfEdgeSwitches() const noexcept { return numEdgeSwitches; }
    count getNumberOfLazy() const noexcept { return numLazy; }
    count getNumberOfSuccessfulInsertionsDeletions() const noexcept {
        return numSuccessfulInsertionsDeletions;
    }
    count getNumberOfSuccessfulHingeFlips() const noexcept { return numSuccessfulHingeFlips; }
    count getNumberOfSuccessfulEdgeSwitches() const noexcept { return numSuccessfulEdgeSwitches; }

    void resetStatistics();

    void setSamplingStrategy(DegreeIntervalSampling s) { samplingStrategy = s; }

    DegreeIntervalSampling getSamplingStrategy() const noexcept { return samplingStrategy; }

private:
    Graph graph;
    std::vector<std::pair<node, node>> degreeIntervals;
    std::discrete_distribution<node> upperDegreeDistribution;

    std::vector<node> nodes;
    std::vector<node>::const_iterator nodesReader;

    DegreeIntervalSampling samplingStrategy{
        DegreeIntervalSampling::DegreeIntervalSampleSingleEdges};

    count numberOfSwitches;

    double probInsertionDeletion = 1.0 / 6;
    double probHingeFlip = 1.0 / 6;
    double probEdgeSwitch = 1.0 / 6;

    count numInsertionsDeletions = 0;
    count numAttemptedInsertions = 0;
    count numAttemptedDeletions = 0;
    count numHingeFlips = 0;
    count numEdgeSwitches = 0;
    count numLazy = 0;

    count numSuccessfulInsertionsDeletions = 0;
    count numSuccessfulInsertions = 0;
    count numSuccessfulDeletions = 0;
    count numSuccessfulHingeFlips = 0;
    count numSuccessfulEdgeSwitches = 0;

    node sampleNodeWeightedByDegree(std::mt19937_64 &);
    node sampleRandomNode(std::mt19937_64 &);

    bool tryPerformSingleInsertionDeletionOnNodes(node u, node v);
    bool tryPerformSingleHingeFlipOnNodes(node u, node v, node w);
    bool tryPerformSingleEdgeSwitchOnNodes(node s1, node t1, node s2, node t2);

    template <typename ISSample, typename HFSample, typename ESSample, typename Skipped>
    void runStrategy(std::mt19937_64 &gen, ISSample iSSample, HFSample hFSampler,
                     ESSample eSSampler, Skipped skipped);

    void runStrategySingleSampleEdges(std::mt19937_64 &gen);
    void runStrategySingleSampleTuples(std::mt19937_64 &gen);
    void runStrategyGlobalSampleTuples(std::mt19937_64 &gen);

    bool canRemoveNeighborOf(node u) const noexcept {
        return degreeIntervals[static_cast<size_t>(u)].first < graph.degree(u);
    }

    bool canAddNeighborTo(node u) const noexcept {
        return degreeIntervals[static_cast<size_t>(u)].second > graph.degree(u);
    }
};

} // namespace NetworKit

#endif // NETWORKIT_RANDOMIZATION_DEGREE_INTERVAL_SWITCHING_HPP_
