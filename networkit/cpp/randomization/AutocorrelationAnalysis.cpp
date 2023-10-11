// networkit-format
/*
 * AutocorrelationAnalysis.cpp
 *
 *  Created on: 11.10.2023
 *      Author: Hung Tran <htran@ae.cs.uni-frankfurt.de>
 */

#include <set>
#include <networkit/randomization/AutocorrelationAnalysis.hpp>

namespace NetworKit {

AutocorrelationAnalysis::AutocorrelationAnalysis(DegreeIntervalSwitching &algorithm_,
                                                 const std::vector<size_t> &thinnings_,
                                                 size_t minSnapshots_,
                                                 const std::string &algorithmLabel_,
                                                 const std::string &graphLabel_,
                                                 const std::string &outputFilename_,
                                                 size_t switchesPerEdge_ = 1,
                                                 size_t maxSnapshots_ = std::numeric_limits<size_t>::max())
    : algorithm(algorithm_),
      numNodes(algorithm.getGraph().numberOfNodes()),
      initialNumEdges(algorithm.getGraph().numberOfEdges()),
      thinnings(thinnings_),
      minSnapshots(minSnapshots_),
      algorithmLabel(algorithmLabel_),
      graphLabel(graphLabel_),
      outputFilename(outputFilename_),
      switchesPerEdge(switchesPerEdge_),
      maxSnapshots(maxSnapshots_),
      numPossibleEdges(numNodes * (numNodes - 1)/2),
      startingSeed(Aux::Random::getSeed())
{
    // compute least common multiple of all thinning values
    const size_t thinningsLcm = std::accumulate(thinnings.begin(), thinnings.end(), 1,
                                              [](size_t a, size_t b) { return std::lcm<size_t>(a, b); });

    // compute set of points where snapshots are to be taken of and materialize in vector
    const auto minSnapshotsFiller = minSnapshots / (thinningsLcm / thinnings.back()) + (minSnapshots % (thinningsLcm / thinnings.back()) != 0);
    minChainLength = std::max(thinningsLcm, minSnapshotsFiller * thinningsLcm);
    std::set<index> snapshotsSet;
    for (const auto thinning : thinnings) {
        for (size_t i = 0; (i < minChainLength / thinning) && (i < maxSnapshots); i++) {
            snapshotsSet.insert((i + 1) * thinning);
        }
    }
    std::copy(snapshotsSet.begin(), snapshotsSet.end(), std::back_inserter(snapshots));

    const Graph &graph = algorithm.getGraph();

    // data structure for the thinnings contains
    // - per possible edge a transition counter
    // - last considered snapshot
    tPrevSnapshots.reserve(thinnings.size());
    tProcSnapshots.reserve(thinnings.size());
    tEdgeTransitions.reserve(thinnings.size());
    tEdgeBits.reserve(thinnings.size());
    for (index thinningIndex = 0; thinningIndex < thinnings.size(); thinningIndex++) {
        tEdgeTransitions.emplace_back(numPossibleEdges);
        tEdgeBits.emplace_back(numPossibleEdges);
    }

    // set initial bits of existing edges to true
    graph.forEdges([&](node u, node v) {
      for (auto &edge_bits : tEdgeBits) {
          assert(get_index(to_edge(u, v)) < tEdgeBits[0].size());
          edge_bits[get_index(to_edge(u, v))] = true;
      }
    });
}

void AutocorrelationAnalysis::run() {
    std::cout
        << "# processing with the following configuration:\n"
        << "# algo " << algorithmLabel << "\n"
        << "# graph " << graphLabel << "\n"
        << "# n " << numNodes << "\n"
        << "# initial m " << initialNumEdges << "\n"
        << "# chain length " << minChainLength << "\n"
        << "# number of snapshots " << snapshots.size() << "\n"
        << "# min number of snapshots " << minSnapshots << "\n"
        << "# max number of snapshots " << maxSnapshots << "\n"
        << "# switches per initial edge " << switchesPerEdge << "\n"
        << "# datastructure size in Bytes  " << sizeof(ThinningCounter) * thinnings.size() * numPossibleEdges
        << std::endl;

    // perform the switchings and add edges to time series
    std::cout << "# performing switches" << std::endl;
    std::vector<count> successfulSwitches(snapshots.size());
    count lastSnapshot = 0;
    for (index snapshotIndex = 0; snapshotIndex < snapshots.size(); snapshotIndex++) {
        std::cout << snapshotIndex << std::endl;
        const index snapshot = snapshots[snapshotIndex];

        // compute requested number of switches, filling the gap from last snapshot to this snapshot
        const auto factor = snapshot - lastSnapshot;
        const auto requested_switches = factor * switchesPerEdge * initialNumEdges;

        // perform switchings
        algorithm.setNumberOfSwitches(requested_switches);
        algorithm.run();
        successful_switches[snapshotIndex] = algorithm.getNumberOfSuccessfulInsertionsDeletions() + algorithm.getNumberOfSuccessfulHingeFlips() + algorithm.getNumberOfSuccessfulEdgeSwitches();
        algorithm.resetStatistics();

        // copy edgelist to bit representation
        std::vector<bool> switchedEdgeBits(numPossibleEdges);
        algorithm.getGraph().forEdges([&](node u, node v) {
          switchedEdgeBits[get_index(to_edge(u, v))] = true;
        });

        // iterate over different thinnings and perform updates
        for (index thinningIndex = 0; thinningIndex < thinnings.size(); thinningIndex++) {
            const auto prevSnapshot = tPrevSnapshots[thinningIndex];
            const auto procSnapshot = tProcSnapshots[thinningIndex];
            const auto thinning = thinnings[thinningIndex];
            if ((prevSnapshot + thinning == snapshot) && (procSnapshot < maxSnapshots)) {
                const auto &edgeBits = tEdgeBits[thinningIndex];
                auto &edgeTransitions = tEdgeTransitions[thinningIndex];
                assert(edgeBits.size() == switchedEdgeBits.size());

                for (index bitIndex = 0; bitIndex < edgeBits.size(); bitIndex++) {
                    const auto prev = edgeBits[bitIndex];
                    const auto next = switchedEdgeBits[bitIndex];
                    edgeTransitions[bitIndex].update(prev, next);
                }

                // update for this thinning last considered snapshot and edgebits
                tPrevSnapshots[thinningIndex] = snapshot;
                tEdgeBits[thinningIndex] = switchedEdgeBits;
                tProcSnapshots[thinningIndex]++;
            }
        }

        lastSnapshot = snapshot;
    }

    // compute snapshots to consider per thinning
    std::vector<std::vector<index>> thinningSnapshots(thinnings.size());
    for (index thinningIndex = 0; thinningIndex < thinnings.size(); thinningIndex++) {
        const auto thinning = thinnings[thinningIndex];
        for (index snapshotIndex = 0; snapshotIndex < snapshots.size() && (thinningSnapshots[thinningIndex].size() < maxSnapshots); snapshotIndex++) {
            const auto snapshot = snapshots[snapshotIndex];
            if (snapshot == (thinningSnapshots[thinningIndex].size() + 1) * thinning) {
                thinningSnapshots[thinningIndex].push_back(snapshotIndex);
            }
        }
    }

    // open file and write out output
    std::ofstream outputFile(outputFilename);
    for (index thinningIndex = 0; thinningIndex < thinnings.size(); thinningIndex++) {
        count thinningSuccessfulSwitches = 0;
        for (index snapshotIndex = 0; snapshotIndex <= thinningSnapshots[thinningIndex].back(); snapshotIndex++) {
            thinningSuccessfulSwitches += successfulSwitches[snapshotIndex];
        }

        // evaluate the delta BIC
        ThinningCounter evaluation;
        const auto &edgeTransitions = tEdgeTransitions[thinningIndex];
        for (const auto &edgeTransition : edgeTransitions) {
            const double deltaBIC = edgeTransition.computeDeltaBIC();
            evaluation.update(edgeTransition.isNone(), deltaBIC);
        }

        // write data for a thinning value to file
        outputFile  << "AUTOCORRELATION,"
                    << thinnings[thinningIndex] << ","
                    << tProcSnapshots[thinningIndex] << ","
                    << thinningSuccessfulSwitches << ","
                    << evaluation.numIndependent << ","
                    << evaluation.numNonIndependent << ","
                    << numPossibleEdges - evaluation.numIndependent - evaluation.numNonIndependent << "," // verify all edges considered TODO remove
                    << startingSeed << "\n";
    }

    outputFile.close();

    hasRun = true;
}
}
