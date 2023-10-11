/*
 * AutocorrelationAnalysisGTest.cpp
 *
 *  Created on: 11.10.2023
 *      Author:  Hung Tran <htran@ae.cs.uni-frankfurt.de>
 */

#include <gtest/gtest.h>

#include <networkit/graph/Graph.hpp>
#include <networkit/graph/GraphTools.hpp>
#include <networkit/generators/ErdosRenyiGenerator.hpp>
#include <networkit/randomization/AutocorrelationAnalysis.hpp>

namespace NetworKit {

class AutocorrelationAnalysisGTest : public testing::Test {
protected:
    void checkWithGraph(const Graph &);
};

void AutocorrelationAnalysisGTest::checkWithGraph(const Graph &G) {
    std::vector<std::pair<node, node>> degreeIntervals;
    degreeIntervals.reserve(G.numberOfNodes());
    G.forNodes([&](node u) {
        const auto deg = G.degree(u);
       degreeIntervals.emplace_back(deg, deg + 3);
    });
    DegreeIntervalSwitching disAlgo(G, degreeIntervals, 1.0);

    std::vector<size_t> thinnings = {1, 2, 3, 5, 7};
    size_t minSnapshots = 100;
    size_t maxSnapshots = 1000;
    AutocorrelationAnalysis autocorrelationAnalysis(disAlgo,
                                                    thinnings,
                                                    minSnapshots,
                                                    "default",
                                                    "GilbertGTest",
                                                    "GilbertGTest.out",
                                                    1,
                                                    maxSnapshots);
    autocorrelationAnalysis.run();
}

TEST_F(AutocorrelationAnalysisGTest, testAutocorrelationAnalysisGilbert) {
    Aux::Random::setSeed(1, false);

    const node numNodes = 100;
    ErdosRenyiGenerator generator(numNodes, 0.3);
    const Graph G = generator.generate();

    std::unordered_map<node, node> nodeIds = std::move(GraphTools::getContinuousNodeIds(G));
    Graph compactedG = GraphTools::getCompactedGraph(G, nodeIds);
    compactedG.sortEdges();
    compactedG.shrinkToFit();
    this->checkWithGraph(compactedG);
}

}