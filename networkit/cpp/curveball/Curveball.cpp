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
	
		std::vector<edge_t> edges;
		_adj_list.getEdges(edges);

		_adj_list.restructure();
		_trade_list.initialize(trades);

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
        neighbour_vector common_neighbours;
        neighbour_vector disjoint_neighbours;

        common_neighbours.reserve(_max_degree);
        disjoint_neighbours.reserve(_max_degree);
		for (const auto& trade : trades) {
			// Trade partners u and v
			const node_t u = trade.first;
			const node_t v = trade.second;

			// Shift the _trade_list offset for these two, currently was set to trade_count
			_trade_list.inc_offset(u);
			_trade_list.inc_offset(v);
			
			// Retrieve respective neighbours
		    // we return whether u has v in his neighbors or vice-versa
		    auto organize_neighbors = [&](node_t u, node_t v) {
			auto pos = std::find(_adj_list.begin(u), _adj_list.end(u), v);
			if (pos == _adj_list.cend(u)) {
				// not found but still sort...
				std::sort(_adj_list.begin(u), _adj_list.end(u));
				return false;
			} else {
				(*_adj_list.end(u)) = v;
				*pos = LISTROW_END;
				std::sort(_adj_list.begin(u), _adj_list.end(u));
				*(_adj_list.end(u) - 1) = *_adj_list.end(u);
				// this datastructure doesn't want LISTROW_ENDs not in the end,
				// therefore write 0 if not the case, because there is an assertion in IMAdjacencyList
				if (_adj_list.end(u) + 1 == _adj_list.begin(u + 1))
					*_adj_list.end(u) = LISTROW_END;
				else
					*_adj_list.end(u) = 0;

				return true;
			}
		    };

			const bool u_share = organize_neighbors(u, v);
			const bool v_share = organize_neighbors(v, u);
			auto u_end = (u_share ? _adj_list.cend(u) - 1 : _adj_list.cend(u));
			auto v_end = (v_share ? _adj_list.cend(v) - 1 : _adj_list.cend(v));

			const bool shared = u_share || v_share;
			// both can't have each other, only inserted in one
			assert((!u_share && !v_share) || (u_share != v_share));

			// No need to keep track of direct positions
			// Get common and disjoint neighbors
			// Here sort and parallel scan
			common_neighbours.clear();
			disjoint_neighbours.clear();
			auto u_nit = _adj_list.cbegin(u);
			auto v_nit = _adj_list.cbegin(v);
			while ((u_nit != u_end) && (v_nit != v_end)) {
				assert(*u_nit != v);
				assert(*v_nit != u);
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
			if (u_nit == u_end)
				disjoint_neighbours.insert(disjoint_neighbours.end(), v_nit, v_end);
			else
				disjoint_neighbours.insert(disjoint_neighbours.end(), u_nit, u_end);

			const degree_t u_setsize = static_cast<degree_t>(u_end - _adj_list.cbegin(u) - common_neighbours.size());
			const degree_t v_setsize = static_cast<degree_t>(v_end - _adj_list.cbegin(v) - common_neighbours.size());
			// v_setsize not necessarily needed

			// Reset fst/snd row
			_adj_list.reset_row(u);
			_adj_list.reset_row(v);

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
