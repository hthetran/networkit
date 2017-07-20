/*
 * IMAdjacencyListMaterialization.cpp
 *
 *  Created on: Jul 18, 2017
 *      Author: Hung Tran
 */
#include "IMAdjacencyListMaterialization.h"
#include "../graph/Graph.h"

namespace NetworKit {

using networkit_adjlist = std::vector< std::vector<node_t> >;
using networkit_edgeweights = std::vector < std::vector<node_t> >;

NetworKit::Graph IMAdjacencyListMaterialization::materialize(const CurveBall::IMAdjacencyList& adj_list) const {
	const node_t numNodes = adj_list.numberOfNodes();
	NetworKit::Graph G(numNodes);
        // Implements "toGraphSequential" of GraphBuilder
	std::vector<NetworKit::count> missingEdgesCounts;

	networkit_edgeweights new_edgeWeights(numNodes);
	networkit_adjlist new_outEdges(numNodes);

	missingEdgesCounts.reserve(numNodes);
	// Add first half of edges
	for (node_t nodeid = 0; nodeid < numNodes; nodeid++) {
		//TODO: is the networkit adjacency list sorted for the neighbours?
		const degree_t degree = adj_list.cend(nodeid) - adj_list.cbegin(nodeid);
		G.outDeg[nodeid] = degree;
		missingEdgesCounts.push_back(adj_list.degreeAt(nodeid) - degree);
		new_outEdges[nodeid].reserve(degree);
		new_edgeWeights[nodeid].resize(degree, 1);
		for (auto it = adj_list.cbegin(nodeid); it != adj_list.cend(nodeid); it++) {
			new_outEdges[nodeid].push_back(*it);
		}
	}
	G.outEdges.swap(new_outEdges);
	// Reserve the space
	G.forNodes([&](node_t v) {
		G.outEdges[v].reserve(G.outDeg[v] + missingEdgesCounts[v]);
	});
	// Copy values
	G.forNodes([&](node_t v) {
		for (degree_t i = 0; i < G.outDeg[v]; i++) {
			const node u = G.outEdges[v][i];
			G.outEdges[u].push_back(v);
		}
	});
	// Sort neighbours
	G.forNodes([&](node_t v) {
		std::sort(G.outEdges[v].begin(), G.outEdges[v].end());
	});
	// Set node degrees
	G.forNodes([&](node_t v) {
		G.outDeg[v] = adj_list.degreeAt(v);
	});
	// Set number of self-loops
	G.storedNumberOfSelfLoops = 0;
	// Set numberOfEdges
	G.m = adj_list.numberOfEdges() / 2;
	// Shrink to fit
	G.shrinkToFit();

	return G;
    }
}
