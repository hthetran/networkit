/*
 * IMAdjacencyListMaterialization.cpp
 *
 *  Created on: Jul 18, 2017
 *      Author: Hung Tran
 */
#include <omp.h>
#include "CurveballMaterialization.h"

namespace NetworKit {

	using networkit_adjlist = std::vector< std::vector<node_t> >;
	using networkit_edgeweights = std::vector < std::vector<node_t> >;

	CurveballMaterialization::CurveballMaterialization(const CurveBall::CurveballAdjacencyList& adj_list)
			: _adj_list(adj_list)
	{ }

	NetworKit::Graph CurveballMaterialization::toGraph(bool autoCompleteEdges, bool parallel) {
		Graph G(_adj_list.numberOfNodes(), false, false);

		if (autoCompleteEdges) {
			if (parallel)
				toGraphParallel(G);
			else
				toGraphSequential(G);
		} else {
			toGraphDirectSwap(G);
		}

		return G;
	}


	void CurveballMaterialization::toGraphDirectSwap(Graph &G) {
		//TODO
	}

	void CurveballMaterialization::toGraphParallel(Graph &G) {
		// 1) insertion of first half is threadsafe
		// 2) insertion of second half is not threadsafe, for now done sequentially

		const node_t numNodes = _adj_list.numberOfNodes();

		std::vector<NetworKit::count> missingEdgesCounts(numNodes, 0);

		networkit_edgeweights new_edgeWeights(numNodes);
		networkit_adjlist new_outEdges(numNodes);

		// Add first half of edges and count missing edges for each node
		G.parallelForNodes([&](node nodeid) {
			const degree_t degree =
					static_cast<degree_t>(_adj_list.cend(nodeid) - _adj_list.cbegin(nodeid));
			G.outDeg[nodeid] = degree;
			missingEdgesCounts[nodeid] = _adj_list.degreeAt(nodeid) - degree;
			new_outEdges[nodeid].reserve(degree);
			new_edgeWeights[nodeid].resize(degree, 1);
			for (auto it = _adj_list.cbegin(nodeid); it != _adj_list.cend(nodeid); it++) {
				new_outEdges[nodeid].push_back(*it);
			}
		});

		G.outEdges.swap(new_outEdges);

		// Reserve the space
		G.parallelForNodes([&](node_t v) {
			G.outEdges[v].reserve(G.outDeg[v] + missingEdgesCounts[v]);
		});

		// Second half of the edges
		G.forNodes([&](node_t v) {
			for (degree_t neighbor_id = 0; neighbor_id < G.outDeg[v]; neighbor_id++) {
				const node u = G.outEdges[v][neighbor_id];
				G.outEdges[u].push_back(v);
			}
		});

		//TODO: is the networkit adjacency list even sorted for the neighbours? if not omit this
		// Sort neighbours
		G.parallelForNodes([&](node_t v) {
			std::sort(G.outEdges[v].begin(), G.outEdges[v].end());
		});

		// Set node degrees
		G.parallelForNodes([&](node_t v) {
			G.outDeg[v] = _adj_list.degreeAt(v);
		});

		// Set number of self-loops
		G.storedNumberOfSelfLoops = 0;

		// Set numberOfEdges
		G.m = _adj_list.numberOfEdges() / 2;

		// Shrink to fit
		G.shrinkToFit();
	}

	void CurveballMaterialization::toGraphSequential(Graph &G) {
		const node_t numNodes = _adj_list.numberOfNodes();

		// Analogue to "toGraphSequential" of GraphBuilder
		std::vector<NetworKit::count> missingEdgesCounts;
		missingEdgesCounts.reserve(numNodes);

		networkit_edgeweights new_edgeWeights(numNodes);
		networkit_adjlist new_outEdges(numNodes);

		// Add first half of edges and count missing edges for each node
		G.forNodes([&](node nodeid) {
			const degree_t degree =
					static_cast<degree_t>(_adj_list.cend(nodeid) - _adj_list.cbegin(nodeid));
			G.outDeg[nodeid] = degree;
			missingEdgesCounts.push_back(_adj_list.degreeAt(nodeid) - degree);
			new_outEdges[nodeid].reserve(degree);
			new_edgeWeights[nodeid].resize(degree, 1);
			for (auto it = _adj_list.cbegin(nodeid); it != _adj_list.cend(nodeid); it++) {
				new_outEdges[nodeid].push_back(*it);
			}
		});

		G.outEdges.swap(new_outEdges);

		// Reserve the space
		G.forNodes([&](node_t v) {
			G.outEdges[v].reserve(G.outDeg[v] + missingEdgesCounts[v]);
		});

		// Second half of the edges
		G.forNodes([&](node_t v) {
			for (degree_t neighbor_id = 0; neighbor_id < G.outDeg[v]; neighbor_id++) {
				const node u = G.outEdges[v][neighbor_id];
				G.outEdges[u].push_back(v);
			}
		});

		//TODO: is the networkit adjacency list even sorted for the neighbours? if not omit this
		// Sort neighbours
		G.forNodes([&](node_t v) {
			std::sort(G.outEdges[v].begin(), G.outEdges[v].end());
		});

		// Set node degrees
		G.forNodes([&](node_t v) {
			G.outDeg[v] = _adj_list.degreeAt(v);
		});

		// Set number of self-loops
		G.storedNumberOfSelfLoops = 0;

		// Set numberOfEdges
		G.m = _adj_list.numberOfEdges() / 2;

		// Shrink to fit
		G.shrinkToFit();
	}
}
