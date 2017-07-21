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
			// Conversion from networkit's edges to ours
			_edgeid_map.insert(std::make_pair((edge_t) edge, id));
			id++;
		}
	}

	void EdgeSwitchingMarkovChainRandomization::run(const edgeswap_vector& swaps) {
		// TODO: get edges by find(id's)
		// TODO: search the swapped by find again
		for (const auto swap : swaps) {
			const edge_t edge_a = _edges[swap.first];
			const edge_t edge_b = _edges[swap.second];
			
			// swapped edges
			const bool dir = (bool) Aux::Random::integer(1);
			const edge_t swapped_a = (dir ? edge_t{edge_a.first, edge_b.first} : edge_t{edge_a.first, edge_b.second}); 
			const edge_t swapped_b = (dir ? edge_t{edge_a.second, edge_b.second} : edge_t{edge_a.second, edge_b.first}); 
		}
	}
}
