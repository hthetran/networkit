/*
 * Curveball.cpp
 *
 *  Created on: Jul 12 2017
 *      Author: Hung Tran
 */

#include "Curveball.h"
#include "IMAdjacencyList.h"
#include "Trade.h"
#include "../auxiliary/Random.h"
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

        IMAdjacencyList _adj_list(degrees, _num_nodes);

		IMTradeList _trade_list(_trades, _num_nodes);

		// insert to adjacency list
        _G.forEdges([&](node_t u, node_t v) {
            if (*(_trade_list.get_trades(u)) < *(_trade_list.get_trades(v))) {
                _adj_list.insert_neighbour(u, v);
            } else {
                if (*(_trade_list.get_trades(u)) > *(_trade_list.get_trades(v))) {
                    _adj_list.insert_neighbour(v, u);
                } else {
                    // Arbitrary tie-breaking
                    if (Aux::Random::integer(1)) {
                        _adj_list.insert_neighbour(u, v);
                    } else {
                        _adj_list.insert_neighbour(v, u);
                    }
                }
            }
        });

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
