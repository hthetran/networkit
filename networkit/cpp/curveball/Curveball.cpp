/*
 * Curveball.cpp
 *
 *  Created on: Jul 12 2017
 *      Author: Hung Tran
 */

#include "Curveball.h"
#include "IMAdjacencyList.h"
#include "Trade.h"
#include "../graph/Graph.h"
#include "../graph/GraphBuilder.h"

namespace CurveBall {

	using degree_vector = std::vector<degree_t>;
	using trade_vector = std::vector<TradeDescriptor>;

	Curveball::Curveball(const NetworKit::Graph& G)
        : _G(G)
		, _num_nodes(G.numberOfNodes())
    { 
		assert(_num_nodes > 0);
	}

	void Curveball::load_from_graph() {
		const auto edges = _G.edges();
		// use forEdges...

		degree_vector degrees;
	   	degrees.reserve(_num_nodes);
		_G.forNodes([&](node_t v) {
			degrees.push_back(_G.degree(v));
		});

		IMTradeList _trade_list(_trades, _num_nodes);

		// insert to adjacency list

		return;
	}

	void Curveball::restructure_graph() {
		//TODO

		return;
	}

	NetworKit::Graph Curveball::getGraph() {
		NetworKit::GraphBuilder gb(_num_nodes);

		for (node_t node_id = 0; node_id < _num_nodes; node_id++) {
			for (auto row_it = _adj_list.cbegin(node_id); *row_it != LISTROW_END; row_it++) {
				gb.addHalfEdge(node_id, *row_it);
			}
		}

		return gb.toGraph(false);
	}

}
