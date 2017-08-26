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
	using nodepair_vector = std::vector< std::pair<node_t, node_t> >;

	EdgeSwitchingMarkovChainRandomization::EdgeSwitchingMarkovChainRandomization(const NetworKit::Graph& G)
		: _G(G)
	{
		_edges.reserve(G.numberOfEdges());
		 neighbors.reserve(G.numberOfNodes());

	        // compute max degree
		//degree_t maxdeg = 0;
		//G.forNodes([&](node_t v){
		//	maxdeg = std::max(maxdeg, G.degree(v));
		//});

		// reserve space
		G.forNodes([&](node_t v) {
                        std::unordered_set<node_t> set;
                        set.reserve(G.degree(v));
                        neighbors.push_back(set);
                });

		// add edges to map
		for (const auto edge : G.edges()){
			if (edge.first < edge.second) {
				_edges.push_back(static_cast<edge_t>(edge));
                                neighbors[edge.first].insert(edge.second);
			} else {
				_edges.push_back({edge.second, edge.first});
                                neighbors[edge.second].insert(edge.first);
			}
		}
	}

	void EdgeSwitchingMarkovChainRandomization::run(const edgeswap_vector& swaps) {
		for (const auto swap : swaps) {
			const edge_t edge_a = _edges[swap.first];
			const edge_t edge_b = _edges[swap.second];

			assert(edge_a.first < edge_a.second);
			assert(edge_b.first < edge_b.second);

			// swapped edges
			const bool dir = Aux::Random::integer(1);
			const edge_t unord_swapped_a = (dir ? edge_t{edge_a.first, edge_b.first} : edge_t{edge_a.first, edge_b.second});
			const edge_t unord_swapped_b = (dir ? edge_t{edge_a.second, edge_b.second} : edge_t{edge_a.second, edge_b.first});

			const edge_t swapped_a = (unord_swapped_a.first < unord_swapped_a.second ? unord_swapped_a : edge_t{unord_swapped_a.second, unord_swapped_a.first});
			const edge_t swapped_b = (unord_swapped_b.first < unord_swapped_b.second ? unord_swapped_b : edge_t{unord_swapped_b.second, unord_swapped_b.first});

			// check for self-loops
			if ((swapped_a.first == swapped_a.second) || (swapped_b.first == swapped_b.second))
				continue;

			// check for edge existence
			assert(_G.hasEdge(swapped_a.first, swapped_a.second) == (neighbors[swapped_a.first].find(swapped_a.second) != neighbors[swapped_a.first].end()));
			assert(_G.hasEdge(swapped_b.first, swapped_b.second) == (neighbors[swapped_b.first].find(swapped_b.second) != neighbors[swapped_b.first].end()));
                        //if (_G.hasEdge(swapped_a.first, swapped_a.second) || _G.hasEdge(swapped_b.first, swapped_b.second))
			if (neighbors[swapped_a.first].find(swapped_a.second) != neighbors[swapped_a.first].end()
                                        || neighbors[swapped_b.first].find(swapped_b.second) != neighbors[swapped_b.first].end())
                                continue;
			else {
				// update now old edges with swapped ones
				// careful, since swapEdge swaps in a particular way
				_edges[swap.first] = swapped_a;
				_edges[swap.second] = swapped_b;

				neighbors[edge_a.first].erase(edge_a.second);
				neighbors[edge_b.first].erase(edge_b.second);

	                        auto p1 = neighbors[swapped_a.first].insert(swapped_a.second);
	                        auto p2 = neighbors[swapped_b.first].insert(swapped_b.second);

				assert(p1.second);
				assert(p2.second);

				// case distinction necessary for the swap method of networkit
				if (!dir) {
					_G.swapEdge(edge_a.first, edge_a.second, edge_b.first, edge_b.second);
					assert(_G.hasEdge(swapped_a.first, swapped_a.second));
					assert(_G.hasEdge(swapped_b.first, swapped_b.second));
					assert(!_G.hasEdge(edge_a.first, edge_a.second));
					assert(!_G.hasEdge(edge_b.first, edge_b.second));
				} else {
					_G.swapEdge(edge_a.second, edge_a.first, edge_b.first, edge_b.second);
					assert(_G.hasEdge(swapped_a.first, swapped_a.second));
					assert(_G.hasEdge(swapped_b.first, swapped_b.second));
					assert(!_G.hasEdge(edge_a.first, edge_a.second));
					assert(!_G.hasEdge(edge_b.first, edge_b.second));
				}
			}
		}
	}

	NetworKit::Graph EdgeSwitchingMarkovChainRandomization::getGraph() const {
		return _G;
	}

	nodepair_vector EdgeSwitchingMarkovChainRandomization::getEdges() const {
		nodepair_vector result(_edges.begin(), _edges.end());
		return result;
	}
}
