/*
 * EdgeSwitchingMarkovChainRandomization.h
 *
 *  Created on: Jul 20, 2017
 *	Author: Hung Tran
 */


#ifndef CB_EDGESWITCHINGMARKOVCHAINRANDOMIZATION_H
#define CB_EDGESWITCHINGMARKOVCHAINRANDOMIZATION_H

#include "../base/Algorithm.h"
#include "../graph/Graph.h"
#include "defs.h"

namespace CurveBall {

using edgeswap_vector = std::vector< std::pair<edgeid_t, edgeid_t> >;

class EdgeSwitchingMarkovChainRandomization : public NetworKit::Algorithm {

public:
	EdgeSwitchingMarkovChainRandomization(const NetworKit::Graph& G);
	
	void run(const edgeswap_vector& swaps);

	// is const return necessary?
	NetworKit::Graph getGraph();
};

}

#endif // CB_EDGESWITCHINGMARKOVCHAINRANDOMIZATION_H
