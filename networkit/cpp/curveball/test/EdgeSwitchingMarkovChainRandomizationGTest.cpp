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

TEST_F(EdgeSwitchingMarkovChainRandomizationGTest, testRun) {
	NetworKit::Graph tG(4);
	tG.addEdge(0, 1);
	tG.addEdge(2, 3);

	EdgeSwitchingMarkovChainRandomization algo(tG);
}

}
