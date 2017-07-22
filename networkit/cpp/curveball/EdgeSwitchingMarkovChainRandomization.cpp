/*
 * EdgeSwitchingMarkovChainRandomization.cpp
 *
 *  Created on: Jul 20, 2017
 *	Author: Hung Tran
 */

#include "EdgeSwitchingMarkovChainRandomization.h"
#include "../auxiliary/Random.h"
#include "../graph/Graph.h"
#include "EdgeVectorMaterialization.h"

namespace CurveBall {

	using edgeswap_vector = std::vector< std::pair<edgeid_t, edgeid_t> >;
	
	EdgeSwitchingMarkovChainRandomization::EdgeSwitchingMarkovChainRandomization(const NetworKit::Graph& G) 
	{
/*		_edges.reserve(G.numberOfEdges());
		edgeid_t id = 0;
		for (const auto edge : G.edges()){
			_edges.push_back((edge_t) edge);
			_edgeid_map.insert(std::make_pair((edge_t) edge, id));
			id++;
		}

		_degrees.reserve(G.numberOfNodes());
		G.forNodes([&](node_t v) {
			_degrees.push_back(G.degree(v));
		});*/
	}

	void EdgeSwitchingMarkovChainRandomization::run(const edgeswap_vector& swaps) {
		/*
		// TODO: get edges by find(id's)
		// TODO: search the swapped by find again
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
			if ((_edgeid_map.find(swapped_a) != _edgeid_map.end()) || (_edgeid_map.find(swapped_b) != _edgeid_map.end()))
				continue;
			else {
				// update now old edges with swapped ones
				_edgeid_map.erase(edge_a);
				_edgeid_map.erase(edge_b);

				_edges[swap.first] = swapped_a;
				_edges[swap.second] = swapped_b;

				_edgeid_map.insert(std::make_pair(swapped_a, swap.first));
				_edgeid_map.insert(std::make_pair(swapped_b, swap.second));
			}
		}*/
	}

	NetworKit::Graph EdgeSwitchingMarkovChainRandomization::getGraph() const {
		/*NetworKit::EdgeVectorMaterialization gb;
		return gb.materialize(_edges, _degrees);*/
		return NetworKit::Graph(5);
	}
}
