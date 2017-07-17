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

namespace CurveBall {

	using degree_vector = std::vector<degree_t>;
	using trade_vector = std::vector<TradeDescriptor>;
	using neighbour_vector = std::vector<node_t>;

	Curveball::Curveball(const NetworKit::Graph& G)
		: _G(G)
		, _num_nodes(G.numberOfNodes())
		, hasRun(false)
	{ 
		assert(G.numberOfNodes == 0);
		assert(_num_nodes > 0);
	}

	void Curveball::load_from_graph(const bool verbose) {
		if (verbose)
			std::cout << "Load from graph:" << std::endl;

		// compute degree sequence
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
		_trade_list.initialize(_trades, _num_nodes);

		// insert to adjacency list
		if (verbose)
			std::cout << "Direct edges:" << std::endl;
		_G.forEdges([&](node_t u, node_t v) {
			update(u, v);
		});
		return;
	}

	void Curveball::restructure_graph(const bool verbose) {
		if (verbose)
			std::cout << "Restructure graph:" << std::endl;

		// degree sequence of G should be the same as _G
		NetworKit::Graph G = getGraph();

		_adj_list.restructure();
		_trade_list.initialize(_trades, _num_nodes);

		// insert to adjacency list
		if (verbose)
			std::cout << "Direct edges according to trades:" << std::endl;
		G.forEdges([&](node_t u, node_t v) {
			update(u, v);
		});
		return;
	}

	void Curveball::run(const trade_vector& trades, const bool verbose) {
		if (verbose)
			std::cout << "===== Algorithm Run ====="<< std::endl;
		_trades = trades;
		if (!hasRun)
			load_from_graph();
		else
			restructure_graph();

		NetworKit::count trade_count = 0;
		for (const auto trade : _trades) {
			if (verbose)
				std::cout << "Processing trade (" << trade_count << "): " << trade << std::endl;

			// It's important to determine, if both share an edge for later communication
			bool shared = false;
		
			const node_t fst = trade.first;
			const node_t snd = trade.second;

			// we shift the trade_list pointer for these two
			// (is currently at trade_count)
			_trade_list.inc_offset(fst);
			_trade_list.inc_offset(snd);
			
			// retrieve neighbours
			if (verbose)
				std::cout << "Neighbours of " << fst << " :" << std::endl;
			neighbour_vector fst_neigh;
			for (auto n_it = _adj_list.cbegin(fst); n_it != _adj_list.cend(fst); n_it++) {
				if (*n_it == snd) {
					shared = true;
					continue;
				}
				fst_neigh.push_back(*n_it);
				if (verbose)
					std::cout << *n_it << " ";
			}
			if (verbose)
				std::cout << std::endl;
			if (verbose)
				std::cout << "Neighbours of " << snd << " :" << std::endl;
			neighbour_vector snd_neigh;
			for (auto n_it = _adj_list.cbegin(snd); n_it != _adj_list.cend(snd); n_it++) {
				if (*n_it == fst) {
					shared = true;
					continue;
				}
				snd_neigh.push_back(*n_it);
				if (verbose)
					std::cout << *n_it << " ";
			}
			if (verbose)
				std::cout << std::endl;
			// we rewrite all neighbours anyway no need to keep track of direct positions
			// get common and disjoint neighbours
			// TODO: here sort and parallel scan, is there something better?
			std::sort(fst_neigh.begin(), fst_neigh.end());
			std::sort(snd_neigh.begin(), snd_neigh.end());
			neighbour_vector common_neigh;
			neighbour_vector disjoint_neigh;

			auto fst_it = fst_neigh.cbegin();
			auto snd_it = snd_neigh.cbegin();
			while ((fst_it != fst_neigh.cend()) && (snd_it != snd_neigh.cend())) {
				if (*fst_it == *snd_it) {
					common_neigh.push_back(*fst_it);
					//std::cout << "common: " << *fst_it << std::endl;
					fst_it++;
					snd_it++;
					continue;
				}
				if (*fst_it > *snd_it) {
					disjoint_neigh.push_back(*snd_it);
					snd_it++;
					continue;
				}
				if (*fst_it < *snd_it) {
					disjoint_neigh.push_back(*fst_it);
					fst_it++;
					continue;
				}
			}
			if (fst_it == fst_neigh.cend())
				disjoint_neigh.insert(disjoint_neigh.end(), snd_it, snd_neigh.cend());
			else
				disjoint_neigh.insert(disjoint_neigh.end(), fst_it, fst_neigh.cend());

			if (verbose) {
				std::cout << "Common neighbours: " << std::endl;
			   	for (const auto common : common_neigh) 
					std::cout << common << " ";
				std::cout << std::endl;
				std::cout << "Disjoint neighbours: " << std::endl;
				for (const auto dis : disjoint_neigh) 
					std::cout << dis << " ";
				std::cout << std::endl;
			}

			// reset fst/snd row
			_adj_list.reset_row(fst);
			_adj_list.reset_row(snd);

			// TODO: best way to perform permutation? std::shuffle then reassign? done here...
			// TODO: optimize, if one is empty nothing needs to be done
			const degree_t fst_sample_size = static_cast<degree_t>(fst_neigh.size() - common_neigh.size());
			const degree_t snd_sample_size = static_cast<degree_t>(snd_neigh.size() - common_neigh.size()); // not nec. needed
			std::shuffle(disjoint_neigh.begin(), disjoint_neigh.end(), Aux::Random::getURNG());
			
			// assign first fst_sample_size to fst and last snd_sample_size to snd
			// if not existent then max value, and below compare goes in favor of partner, if partner
			// has no more neighbours as well then their values are equal (max and equal)
			// and tiebreaking is applied
			for (degree_t counter = 0; counter < fst_sample_size; counter++) {
				const node_t swapped = disjoint_neigh[counter];
				update(fst, swapped);
			}
			for (degree_t counter = fst_sample_size; counter < fst_sample_size + snd_sample_size; counter++) {
				const node_t swapped = disjoint_neigh[counter];
				update(snd, swapped);
			}
			// Distribute common edges
			for (const auto common : common_neigh) {
				update(fst, common);
				update(snd, common);
			}
			// Do not forget edge between fst and snd......................(!!!!) if it exists...........
			if (shared)
				update(fst, snd);

			trade_count++;
		}

		hasRun = true;

		return;
	}

	void Curveball::update(const node_t a, const node_t b, bool verbose) {
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
