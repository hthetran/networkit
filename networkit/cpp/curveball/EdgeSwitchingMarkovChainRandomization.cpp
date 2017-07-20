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
	using nodepair = std::pair<node_t, node_t>;

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
		for (const auto swap : swaps) {
			// ids of edges to swap
			const edgeid_t a = swap.first;
			const edgeid_t b = swap.second;

			// edge (iterators) with these ids
			auto edgeit_a = _edges_map.find(a);
			auto edgeit_b = _edges_map.find(b);

			// edges
			const nodepair edge_a = (*edgeit_a).second;
			const nodepair edge_b = (*edgeit_b).second;

			// swap direction
			if (Aux::Random::integer(1)) {
				const nodepair swapped_a = std::make_pair(edge_a.first, edge_b.first);
				const nodepair swapped_b = std::make_pair(edge_a.second, edge_b.second);	
			} else {
				const nodepair swapped_a = std::make_pair(edge_a.first, edge_b.second);
				const nodepair swapped_b = std::make_pair(edge_a.second, edge_b.first);
			}
		}
	}
}
