/*
 * EdgeSwitchingMarkovChainRandomization.cpp
 *
 *  Created on: Jul 20, 2017
 *	Author: Hung Tran
 */

#include "EdgeSwitchingMarkovChainRandomization.h"
#include "../auxiliary/Random.h"
#include "../graph/Graph.h"

namespace CurveBall {

	using edgeswap_vector = std::vector< std::pair<edgeid_t, edgeid_t> >;
	
	EdgeSwitchingMarkovChainRandomization::EdgeSwitchingMarkovChainRandomization(const NetworKit::Graph& G) 
		: _G(G)
	{ 
		_edges.reserve(G.numberOfEdges());
		for (const auto edge : G.edges()){
			_edges.push_back(static_cast<edge_t>(edge));
		}

		_degrees.reserve(G.numberOfNodes());
		G.forNodes([&](node_t v) {
			_degrees.push_back(G.degree(v));
		});
	}

	void EdgeSwitchingMarkovChainRandomization::run(const edgeswap_vector& swaps) {
		for (const auto swap : swaps) {
			const edge_t edge_a = _edges[swap.first];
			const edge_t edge_b = _edges[swap.second];
			
			// swapped edges
			const bool dir = (bool) Aux::Random::integer(1);
			const edge_t swapped_a = (dir ? edge_t{edge_a.first, edge_b.first} : edge_t{edge_a.first, edge_b.second}); 
			const edge_t swapped_b = (dir ? edge_t{edge_a.second, edge_b.second} : edge_t{edge_a.second, edge_b.first});

			// check for self-loops
			if (swapped_a.is_invalid() || swapped_b.is_invalid())
				continue;
			
			// check for edge existence
			if (_G.hasEdge(swapped_a.first, swapped_a.second) || _G.hasEdge(swapped_b.first, swapped_b.second))
				continue;
			else {
				// update now old edges with swapped ones
				// careful, since swapEdge swaps in a particular way
				_edges[swap.first] = swapped_a;
				_edges[swap.second] = swapped_b;
				if (!dir)
					_G.swapEdge(edge_a.first, edge_a.second, edge_b.first, edge_b.second);
				else
					_G.swapEdge(edge_a.second, edge_a.first, edge_b.first, edge_b.second);
			}
		}
	}

	NetworKit::Graph EdgeSwitchingMarkovChainRandomization::getGraph() const {
		return _G;
	}
}
