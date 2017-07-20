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
#include <stx/btree_map.h>

namespace CurveBall {

using edgeswap_vector = std::vector< std::pair<edgeid_t, edgeid_t> >;
using edge_vector = std::vector< std::pair<node_t, node_t> >;

class EdgeSwitchingMarkovChainRandomization : public NetworKit::Algorithm {

protected:
	edge_vector _edges;
	stx::btree_map<edgeid_t, std::pair<node_t, node_t> > _edges_map;

public:
	EdgeSwitchingMarkovChainRandomization(const NetworKit::Graph& G);

	void run() {
		std::runtime_error("Cannot use this method, algorithm needs to receive swaps");
	}

	void run(const edgeswap_vector& swaps);

	// is const return necessary?
	// TODO: EdgeVectorMaterialization
	NetworKit::Graph getGraph();
};

}

#endif // CB_EDGESWITCHINGMARKOVCHAINRANDOMIZATION_H
