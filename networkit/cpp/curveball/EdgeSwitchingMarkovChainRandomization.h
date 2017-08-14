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
#include <unordered_set>

namespace CurveBall {

using edgeswap_vector = std::vector< std::pair<edgeid_t, edgeid_t> >;
using edge_vector = std::vector<edge_t>;
using degree_vector = std::vector<degree_t>;
using nodepair_vector = std::vector< std::pair<node_t, node_t> >;

template <size_t max>
class edge_hash {
	std::size_t operator () (const edge_t &e) const {
		std::hash<node_t> node_hash;
		auto h1 = node_hash(e.first);
		auto h2 = node_hash(e.second);

		// combine hashes here, boost style
		h1 ^= h2 + 0x9e3779b9 + (h1<<6) + (h1>>2);

		return h1;
	}
};

using hashmap_vector = std::vector<std::unordered_set<node_t>>;

class EdgeSwitchingMarkovChainRandomization : public NetworKit::Algorithm {

protected:
	edge_vector _edges;
	NetworKit::Graph _G;
	hashmap_vector neighbors;

public:
	EdgeSwitchingMarkovChainRandomization(const NetworKit::Graph& G);

	void run() {
		std::runtime_error("Cannot use this method, algorithm needs to receive swaps");
	}

	void run(const edgeswap_vector& swaps);

	NetworKit::Graph getGraph() const;

	nodepair_vector getEdges() const;
};

}

#endif // CB_EDGESWITCHINGMARKOVCHAINRANDOMIZATION_H
