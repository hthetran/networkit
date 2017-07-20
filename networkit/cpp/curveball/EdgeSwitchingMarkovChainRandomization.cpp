/*
 * EdgeSwitchingMarkovChainRandomization.cpp
 *
 *  Created on: Jul 20, 2017
 *	Author: Hung Tran
 */

#include "EdgeSwitchingMarkovChainRandomization.h"
#include "../auxiliary/Random.h"

namespace CurveBall {

	using edgeswap_vector = std::vector< std::pair<edgeid_t, edgeid_t> >;

	EdgeSwitchingMarkovChainRandomization::EdgeSwitchingMarkovChainRandomization(const NetworKit::Graph& G) 
		: _edges(G.edges())
	{
		edgeid_t id = 0;
		for (const edge_t edge : _edges) {
			_edges_map.insert(std::make_pair(id, edge));
			id++;
		}
	}

	void EdgeSwitchingMarkovChainRandomization::run(const edgeswap_vector& swaps) {
		// TODO: get edges by find(id's)
		// TODO: search the swapped by find again	
	}
}
