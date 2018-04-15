/*
 * CurveballIM.cpp
 *
 *  Created on: Jul 12 2017
 *      Author: Hung Tran
 */

#include <iostream>
#include "CurveballIM.h"
#include "../graph/GraphBuilder.h"
#include "CurveballMaterialization.h"
#include "../auxiliary/SignalHandling.h"

namespace CurveballImpl {

	using degree_vector = std::vector<degree_t>;
	using trade_vector = std::vector<TradeDescriptor>;
	using neighbour_vector = std::vector<node_t>;
	using node_vector = std::vector<node_t>;
	using nodepair_vector = std::vector< std::pair<node_t, node_t> >;

	CurveballIM::CurveballIM(const NetworKit::Graph& G)
		: CurveballBase(G)
		, _trade_list(G.numberOfNodes())
		, _aff_edges(0)
	{
		hasRun = false;
		assert(G.checkConsistency());
		assert(G.numberOfSelfLoops() == 0);
		assert(_num_nodes > 0);
	}

	void CurveballIM::load_from_graph(const trade_vector& trades) {
		// Compute degree sequence
		degree_vector degrees;
		degrees.reserve(_num_nodes);
		edgeid_t degree_sum = 0;
		_G.forNodes([&](node_t v) {
				degrees.push_back(_G.degree(v));
				degree_sum += _G.degree(v);
				});

		_max_degree = *(std::max_element(degrees.cbegin(), degrees.cend()));

		_adj_list.initialize(degrees, degree_sum);
		_trade_list.initialize(trades);

		// Insert to adjacency list, directed according trades
		_G.forEdges([&](node_t u, node_t v) {
				update(u, v);
				});
		return;
	}

	void CurveballIM::restructure_graph(const trade_vector& trades) {
		nodepair_vector edges =_adj_list.getEdges();

		_adj_list.restructure();
		_trade_list.initialize(trades);

		for (const auto edge : edges) {
			update(edge.first, edge.second);
		}

		return;
	}

	void CurveballIM::run(const trade_vector& trades) {
		if (!hasRun)
			load_from_graph(trades);
		else
			restructure_graph(trades);

		NetworKit::count trade_count = 0;
		neighbour_vector common_neighbours;
		neighbour_vector disjoint_neighbours;

		common_neighbours.reserve(_max_degree);
		disjoint_neighbours.reserve(_max_degree);

		Aux::SignalHandler handler;

		for (const auto& trade : trades) {
			handler.assureRunning();

			// Trade partners u and v
			const node_t u = trade.first;
			const node_t v = trade.second;

			_aff_edges += _adj_list.degreeAt(u);
			_aff_edges += _adj_list.degreeAt(v);

			// Shift the _trade_list offset for these two, currently was set to trade_count
			_trade_list.incrementOffset(u);
			_trade_list.incrementOffset(v);

			// Retrieve respective neighbours
			// we return whether u has v in his neighbors or vice-versa
			auto organize_neighbors = [&](node_t node_x, node_t node_y) {
				auto pos = std::find(_adj_list.begin(node_x), _adj_list.end(node_x), node_y);
				if (pos == _adj_list.cend(node_x)) {
					// element not found, sort anyway
					std::sort(_adj_list.begin(node_x), _adj_list.end(node_x));

					return false;
				} else {
					// overwrite node_y's position with END
					*pos = LISTROW_END;

					// sort, such that node_y's position is at end - 1
					std::sort(_adj_list.begin(node_x), _adj_list.end(node_x));

					// overwrite with node_y again
					*(_adj_list.end(node_x) - 1) = node_y;

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
			_adj_list.resetRow(u);
			_adj_list.resetRow(v);

			std::shuffle(disjoint_neighbours.begin(), disjoint_neighbours.end(), Aux::Random::getURNG());

			// Assign first u_setsize to u and last v_setsize to v
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

	edgeid_t CurveballIM::getNumberOfAffectedEdges() const {
		return _aff_edges;
	}

	NetworKit::Graph CurveballIM::getGraph() const {
		NetworKit::CurveballMaterialization gb(_adj_list);

		return gb.toGraph(true, false);
	}

	nodepair_vector CurveballIM::getEdges() const {
		return _adj_list.getEdges();
	}
}
