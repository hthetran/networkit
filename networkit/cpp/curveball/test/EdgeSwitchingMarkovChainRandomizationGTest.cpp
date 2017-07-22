/*
 * EdgeSwitchingMarkovChainRandomizationGTest.cpp
 *
 *  Created on: Jul 20, 2017
 *	Author: Hung Tran
 */

#include "EdgeSwitchingMarkovChainRandomizationGTest.h"
#include "../../graph/Graph.h"
#include "../EdgeSwitchingMarkovChainRandomization.h"

namespace CurveBall {

using edgeswap_vector = std::vector< std::pair<edgeid_t, edgeid_t> >;

TEST_F(EdgeSwitchingMarkovChainRandomizationGTest, testRunSingleTrade) {
	NetworKit::Graph tG(4);
	tG.addEdge(0, 1);
	tG.addEdge(2, 3);

	EdgeSwitchingMarkovChainRandomization algo(tG);
	
	const edgeswap_vector swaps = { std::make_pair(0, 1) };

	algo.run(swaps);

	NetworKit::Graph Gout = algo.getGraph();
	if (Gout.hasEdge(0, 2)) 
		ASSERT_TRUE(Gout.hasEdge(1, 3));
	else {
		ASSERT_TRUE(Gout.hasEdge(0, 3));
		ASSERT_TRUE(Gout.hasEdge(1, 2));
	}
}

}
