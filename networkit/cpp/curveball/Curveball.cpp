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
	{
		hasRun = false;
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

		_max_degree = *(std::max_element(degrees.cbegin(), degrees.cend()));

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
		if (!hasRun)
			load_from_graph(trades);
		else
			restructure_graph(trades);

		NetworKit::count trade_count = 0;
		neighbour_vector u_neighbours;
		neighbour_vector v_neighbours;
        neighbour_vector common_neighbours;
        neighbour_vector disjoint_neighbours;

        u_neighbours.reserve(_max_degree);
        v_neighbours.reserve(_max_degree);
        common_neighbours.reserve(_max_degree);
        disjoint_neighbours.reserve(_max_degree);
		for (const auto& trade : trades) {
			// It's important to determine, if both share an edge for later communication
			bool shared = false;
	
			// Trade partners u and v
			const node_t u = trade.first;
			const node_t v = trade.second;

			// Shift the _trade_list offset for these two, currently was set to trade_count
			_trade_list.inc_offset(u);
			_trade_list.inc_offset(v);
			
			// Retrieve respective neighbours

                        // Manuel: why do you copy the values? Is it not possible to operate on
                        // on _adj_list.begin/end() directly?

                        // Manuel: If you do not copy, you may use std::search to find shared (and swap it with end to remove it)

                        // Manuel: You can use lambdas to avoid redudancy
                        //  auto copyVector = [&] (node u) { ... };
                        //  u_neigh = copyVector(u);
                        //  v_neigh = copyVector(v);
            const bool myshared = (std::find(_adj_list.cbegin(u), _adj_list.cend(u), v) != _adj_list.cend(u))
                                  || (std::find(_adj_list.cbegin(v), _adj_list.cend(v), u) != _adj_list.cend(v));
            auto organize_nodes = [&](node_t u, node_t v) {
                std::cout << "u: " << u << std::endl;
                std::cout << "v: " << v << std::endl;

                for (auto it = _adj_list.cbegin(u); it != _adj_list.cend(u); it++)
                    std::cout << *it << std::endl;

                std::cout << "===" << std::endl;
                std::cout << *_adj_list.cend(u)<< std::endl;
                std::cout << "=====================" << std::endl;
                if (u != _num_nodes - 1)
                    for(auto it = _adj_list.cbegin(u); it != _adj_list.cbegin(u + 1); it++)
                        std::cout << *it << std::endl;
                std::cout << "=====================" << std::endl;
                std::cout << "=" << std::endl;
                auto pos = std::find(_adj_list.begin(u), _adj_list.end(u), v);
                std::cout << "What is found? " << *pos << std::endl;
                std::cout << "=" << std::endl;
                // TODO: swap with begin(u + 1) - 1; that has the sentinel! otherwise it can happen that one swaps with uninitialized location with a zero!
/*
                auto pos = std::find(_adj_list.begin(u), _adj_list.end(u), v);
                std::cout << "Setting end(u) - 1 which was " << *(_adj_list.end(u) - 1) << " to *pos which is " << *pos << std::endl;
                *(_adj_list.end(u)) = *pos;
                *pos = LISTROW_END;
                std::sort(_adj_list.begin(u), _adj_list.end(u) - 1);
                *(_adj_list.end(u) - 1) = *(_adj_list.end(u));
                *(_adj_list.end(u)) = LISTROW_END;

                std::cout << "==========" << std::endl;

                for (auto it = _adj_list.cbegin(u); it != _adj_list.cend(u); it++)
                    std::cout << *it << std::endl;*/
            };

			u_neighbours.clear();
            v_neighbours.clear();
            organize_nodes(u, v);
            organize_nodes(v, u);
			for (auto n_it = _adj_list.cbegin(u); n_it != _adj_list.cend(u); n_it++) {
				if (*n_it == v) {
					shared = true;
					continue;
				}
				u_neighbours.push_back(*n_it);
			}
			for (auto n_it = _adj_list.cbegin(v); n_it != _adj_list.cend(v); n_it++) {
				if (*n_it == u) {
					shared = true;
					continue;
				}
				v_neighbours.push_back(*n_it);
			}

            assert(myshared == shared);

			// No need to keep track of direct positions
			// Get common and disjoint neighbours
			// TODO: here sort and parallel scan, is there something better?
			std::sort(u_neighbours.begin(), u_neighbours.end());
			std::sort(v_neighbours.begin(), v_neighbours.end());

            common_neighbours.clear();
            disjoint_neighbours.clear();
			auto u_nit = u_neighbours.cbegin();
			auto v_nit = v_neighbours.cbegin();
			while ((u_nit != u_neighbours.cend()) && (v_nit != v_neighbours.cend())) {
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
				// *u_nit == *v_nit
				{
					common_neighbours.push_back(*u_nit);
					u_nit++;
					v_nit++;
				}
			}
			if (u_nit == u_neighbours.cend())
				disjoint_neighbours.insert(disjoint_neighbours.end(), v_nit, v_neighbours.cend());
			else
				disjoint_neighbours.insert(disjoint_neighbours.end(), u_nit, u_neighbours.cend());

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

	NetworKit::Graph Curveball::getGraph(bool verbose) const {
		const NetworKit::IMAdjacencyListMaterialization gb;
		return gb.materialize(_adj_list);
	}
}
