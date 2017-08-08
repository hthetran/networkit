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
		edgeid_t count = 0;
		edgeidmap.reserve(G.numberOfEdges());
		for (const auto edge : G.edges()){
			if (edge.first < edge.second) {
				_edges.push_back(static_cast<edge_t>(edge));
				edgeidmap.insert({static_cast<edge_t>(edge), count});
			} else {
				_edges.push_back({edge.second, edge.first});
				edgeidmap.insert({edge_t(edge.second, edge.first), count});
			}	
			count++;
		}
	}

	void EdgeSwitchingMarkovChainRandomization::run(const edgeswap_vector& swaps) {
		std::cout << "edges size " << _edges.size() << std::endl;

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

			std::cout << "swapped a " << swapped_a << std::endl;
			std::cout << "swapped b " << swapped_b << std::endl;
			
			// check for self-loops
			if ((swapped_a.first == swapped_a.second) || (swapped_b.first == swapped_b.second))
				continue;
			
			// check for edge existence
			std::cout << "HM size: " << edgeidmap.size() << std::endl;
			std::cout << "swapped_a NK: " << _G.hasEdge(swapped_a.first, swapped_a.second) << std::endl;
			std::cout << "swapped_a HM: " << (edgeidmap.find(swapped_a) != edgeidmap.end()) << std::endl;
			std::cout << "swapped_b NK: " << _G.hasEdge(swapped_b.first, swapped_b.second) << std::endl;
			std::cout << "swapped_b HM: " << (edgeidmap.find(swapped_b) != edgeidmap.end()) << std::endl;
			assert(_G.hasEdge(swapped_a.first, swapped_a.second) == (edgeidmap.find(swapped_a) != edgeidmap.end()));
			assert(_G.hasEdge(swapped_b.first, swapped_b.second) == (edgeidmap.find(swapped_b) != edgeidmap.end()));
			if (_G.hasEdge(swapped_a.first, swapped_a.second) || _G.hasEdge(swapped_b.first, swapped_b.second))
				continue;
			else {
				// update now old edges with swapped ones
				// careful, since swapEdge swaps in a particular way
				_edges[swap.first] = swapped_a;
				_edges[swap.second] = swapped_b;
				
				edgeidmap.erase(edge_a);
				edgeidmap.erase(edge_b);
				auto p1 = edgeidmap.insert({swapped_a, swap.first});
				auto p2 = edgeidmap.insert({swapped_b, swap.second});
		
				assert(p1.second);
				assert(p2.second);
				assert(edgeidmap.find(swapped_a) != edgeidmap.end());
				assert(edgeidmap.find(swapped_b) != edgeidmap.end());


				std::cout << "After swap HM size: " << edgeidmap.size() << std::endl;
			
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

				std::cout << "A swapped_a NK: " << _G.hasEdge(swapped_a.first, swapped_a.second) << std::endl;
				std::cout << "A swapped_a HM: " << (edgeidmap.find(swapped_a) != edgeidmap.end()) << std::endl;
				std::cout << "A swapped_b NK: " << _G.hasEdge(swapped_b.first, swapped_b.second) << std::endl;
				std::cout << "A swapped_b HM: " << (edgeidmap.find(swapped_b) != edgeidmap.end()) << std::endl;
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
