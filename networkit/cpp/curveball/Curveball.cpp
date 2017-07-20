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
#include "../Globals.h"
#include "IMAdjacencyListMaterialization.h"

namespace CurveBall {

	using degree_vector = std::vector<degree_t>;
	using trade_vector = std::vector<TradeDescriptor>;
	using neighbour_vector = std::vector<node_t>;
        using node_vector = std::vector<node_t>;

	Curveball::Curveball(const NetworKit::Graph& G)
		: _G(G)
		, _num_nodes(G.numberOfNodes())
		, _trade_list(G.numberOfNodes())
                , hasRun(false)
	{
                assert(G.checkConsistency());
		assert(G.numberOfSelfLoops() == 0);
		assert(_num_nodes > 0);
	}

	void Curveball::load_from_graph(const trade_vector& trades, const bool verbose) {
		if (verbose)
			std::cout << "Load from graph:" << std::endl;

		// Compute degree sequence
		degree_vector degrees;
	   	degrees.reserve(_num_nodes);
		edgeid_t degree_sum = 0;
		_G.forNodes([&](node_t v) {
			degrees.push_back(_G.degree(v));
			degree_sum += _G.degree(v);
		});
		if (verbose)
			std::cout << "Computed degree sequence..." << std::endl;

		_adj_list.initialize(degrees, degree_sum);
		_trade_list.initialize(trades);

		if (verbose)
			std::cout << "Direct edges:" << std::endl;
		
                // Insert to adjacency list, directed according trades
		_G.forEdges([&](node_t u, node_t v) {
			update(u, v);
		});
		return;
	}

	void Curveball::restructure_graph(const trade_vector& trades, const bool verbose) {
		if (verbose)
			std::cout << "Restructure graph:" << std::endl;
	
		std::vector<edge_t> edges;
		_adj_list.getEdges(edges);

		_adj_list.restructure();
		_trade_list.initialize(trades);

		if (verbose)
			std::cout << "Direct edges according to trades:" << std::endl;
		
		for (const auto edge : edges) {
			update(edge.first, edge.second);
		}

		return;
	}

	void Curveball::run(const trade_vector& trades) {
		const bool verbose = false;		

		if (verbose)
			std::cout << "===== Algorithm Run ====="<< std::endl;
		if (!hasRun)
			load_from_graph(trades);
		else
			restructure_graph(trades);

		NetworKit::count trade_count = 0;
		for (const auto trade : trades) {
			if (verbose)
				std::cout << "Processing trade (" << trade_count << "): " << trade << std::endl;

			// It's important to determine, if both share an edge for later communication
			bool shared = false;
	
			// Trade partners u and v	
                        const node_t u = trade.first;
			const node_t v = trade.second;

			// Shift the _trade_list offset for these two, currently was set to trade_count
			_trade_list.inc_offset(u);
			_trade_list.inc_offset(v);
			
			// Retrieve respective neighbours
			if (verbose)
				std::cout << "Neighbours of " << u << " :" << std::endl;
			neighbour_vector u_neighbours;
			for (auto n_it = _adj_list.cbegin(u); n_it != _adj_list.cend(u); n_it++) {
				if (*n_it == v) {
					shared = true;
					continue;
				}
				u_neighbours.push_back(*n_it);
				if (verbose)
					std::cout << *n_it << " ";
			}
			if (verbose)
				std::cout << std::endl;
			if (verbose)
				std::cout << "Neighbours of " << v << " :" << std::endl;
			neighbour_vector v_neighbours;
			for (auto n_it = _adj_list.cbegin(v); n_it != _adj_list.cend(v); n_it++) {
				if (*n_it == u) {
					shared = true;
					continue;
				}
				v_neighbours.push_back(*n_it);
				if (verbose)
					std::cout << *n_it << " ";
			}
			if (verbose)
				std::cout << std::endl;
			
			// No need to keep track of direct positions
			// Get common and disjoint neighbours
			// TODO: here sort and parallel scan, is there something better?
			std::sort(u_neighbours.begin(), u_neighbours.end());
			std::sort(v_neighbours.begin(), v_neighbours.end());
			neighbour_vector common_neighbours;
			neighbour_vector disjoint_neighbours;

			auto u_nit = u_neighbours.cbegin();
			auto v_nit = v_neighbours.cbegin();
			while ((u_nit != u_neighbours.cend()) && (v_nit != v_neighbours.cend())) {
				if (*u_nit == *v_nit) {
					common_neighbours.push_back(*u_nit);
					//std::cout << "common: " << *u_nit << std::endl;
					u_nit++;
					v_nit++;
					continue;
				}
				if (*u_nit > *v_nit) {
					disjoint_neighbours.push_back(*v_nit);
					v_nit++;
					continue;
				}
				if (*u_nit < *v_nit) {
					disjoint_neighbours.push_back(*u_nit);
					u_nit++;
					continue;
				}
			}
			if (u_nit == u_neighbours.cend())
				disjoint_neighbours.insert(disjoint_neighbours.end(), v_nit, v_neighbours.cend());
			else
				disjoint_neighbours.insert(disjoint_neighbours.end(), u_nit, u_neighbours.cend());

			if (verbose) {
				std::cout << "Common neighbours: " << std::endl;
				for (const auto common : common_neighbours) 
					std::cout << common << " ";
				std::cout << std::endl;
				std::cout << "Disjoint neighbours: " << std::endl;
				for (const auto dis : disjoint_neighbours) 
					std::cout << dis << " ";
				std::cout << std::endl;
			}

			// Reset fst/snd row
			_adj_list.reset_row(u);
			_adj_list.reset_row(v);

			const degree_t u_setsize = static_cast<degree_t>(u_neighbours.size() - common_neighbours.size());
			const degree_t v_setsize = static_cast<degree_t>(v_neighbours.size() - common_neighbours.size()); // not nec. needed
			std::shuffle(disjoint_neighbours.begin(), disjoint_neighbours.end(), Aux::Random::getURNG());
			
			// Assign first u_setsize to fst and last v_setsize to snd
			// if not existent then max value, and below compare goes in favor of partner, if partner
			// has no more neighbours as well then their values are equal (max and equal)
			// and tiebreaking is applied
			for (degree_t counter = 0; counter < u_setsize; counter++) {
				const node_t swapped = disjoint_neighbours[counter];
				update(u, swapped);
			}
			for (degree_t counter = u_setsize; counter < u_setsize + v_setsize; counter++) {
				const node_t swapped = disjoint_neighbours[counter];
				update(v, swapped);
			}
			// Distribute common edges
			for (const auto common : common_neighbours) {
				update(u, common);
				update(v, common);
			}
			// Do not forget edge between u and v
			if (shared)
				update(u, v);

			trade_count++;
		}

		hasRun = true;

		return;
	}

	inline void Curveball::update(const node_t a, const node_t b, bool verbose) {
		const tradeid_t ta = *(_trade_list.get_trades(a));
		const tradeid_t tb = *(_trade_list.get_trades(b));
		if (ta < tb) {
			if (verbose)
				std::cout << "node a: " << a << " [" << ta << "] before " << "node b: " << b << " [" << tb << "]" << std::endl;
			_adj_list.insert_neighbour(a, b);
		} else {
			if (ta > tb) {
				if (verbose)
					std::cout << "node a: " << a << " [" << ta << "] after " << "node b: " << b << " [" << tb << "]" << std::endl;
				_adj_list.insert_neighbour(b, a);
			} else {
				if (verbose)
					std::cout << "node a: " << a << " [" << ta << "] again with " << "node b: " << b << " [" << tb << "]" << std::endl;
				if (Aux::Random::integer(1))
					_adj_list.insert_neighbour(a, b);
				else
					_adj_list.insert_neighbour(b, a);
			}
		}
	}
/*
 *  Deprecated using slow GraphBuilder bypass.
	NetworKit::Graph Curveball::getGraph(bool verbose) const {
		NetworKit::GraphBuilder gb(_num_nodes);

                if (verbose)
                    std::cout << "Building graph" << std::endl;

		for (node_t node_id = 0; node_id < _num_nodes; node_id++) {
		    if (verbose)
                        std::cout << "At node: " << node_id << std::endl; 
                    for (auto row_it = _adj_list.cbegin(node_id); row_it != _adj_list.cend(node_id); row_it++) {
                        //if (verbose)
                        //    std::cout << "Neighbour: " << *row_it << std::endl;
                        gb.addHalfEdge(node_id, *row_it);
		    }
		}

		return gb.toGraph(true);
	}*/

	// previously getMaterializedGraph
	// sadly not const return
	NetworKit::Graph Curveball::getGraph(bool verbose) {
		//for (node_t nid = 0; nid < _num_nodes; nid++) {
		//	_adj_list.sortRow(nid);
		//}

		NetworKit::IMAdjacencyListMaterialization gb;
		return gb.materialize(_adj_list);
	}
}
