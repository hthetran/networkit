/*
 * Curveball.cpp
 *
 *  Created on: Jul 12 2017
 *      Author: Hung Tran
 */

#include <iostream>
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
		, hasRun(false)
    { 
		assert(_num_nodes > 0);
	}

	void Curveball::load_from_graph() {
		const auto edges = _G.edges();

		degree_vector degrees;
	   	degrees.reserve(_num_nodes);
		edgeid_t degree_sum = 0;
		_G.forNodes([&](node_t v) {
			degrees.push_back(_G.degree(v));
			degree_sum += _G.degree(v);
		});

        _adj_list.initialize(degrees, degree_sum);

		_trade_list.initialize(_trades, _num_nodes);

		// insert to adjacency list
        _G.forEdges([&](node_t u, node_t v) {
			//std::cout << "node u: " << u << ", node v: " << v << std::endl;
            if (*(_trade_list.get_trades(u)) < *(_trade_list.get_trades(v))) {
				std::cout << "Node: " << u << " is traded before Node: " << v << std::endl;
				_adj_list.insert_neighbour(u, v);
            } else {
                if (*(_trade_list.get_trades(u)) > *(_trade_list.get_trades(v))) {
					//std::cout << "Node: " << u << " is traded after Node: " << v << std::endl;
                    _adj_list.insert_neighbour(v, u);
                } else {
                    // Arbitrary tie-breaking
					//std::cout << "Node: " << u << " is traded at the same time as Node: " << v << std::endl;
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

	void Curveball::run(const trade_vector& trades) {
		_trades = trades;
		if (!hasRun)
			load_from_graph();
		else
			restructure_graph();

		for (const auto trade : _trades) {
			const node_t fst = trade.fst();
			const node_t snd = trade.snd();
		
			// best way to shuffle stuff?	
		}

		hasRun = true;

		return;
	}

	NetworKit::Graph Curveball::getGraph() {
		NetworKit::GraphBuilder gb(_num_nodes);

		for (node_t node_id = 0; node_id < _num_nodes; node_id++) {
			for (auto row_it = _adj_list.cbegin(node_id); row_it != _adj_list.cend(node_id); row_it++) {
				gb.addHalfEdge(node_id, *row_it);
			}
		}

		return gb.toGraph(true);
	}

}
