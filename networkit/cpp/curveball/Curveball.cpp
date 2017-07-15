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
		//else
		//	restructure_graph();

		NetworKit::count trade_count = 0;
		for (const auto trade : _trades) {
			std::cout << "Processing trade: " << trade << std::endl;
			const node_t fst = trade.fst();
			const node_t snd = trade.snd();
			// we shift the trade_list pointer for these two
			_trade_list.shift_offset(fst, 1);
			_trade_list.shift_offset(snd, 1);
			
			// TODO: retrieve neighbours, isn't this a painful act with non-sorted entries?
			neighbour_vector fst_neigh;
			for (auto n_it = _adj_list.cbegin(fst); n_it != _adj_list.cend(fst); n_it++) {
				if (*n_it == snd)
					continue;
				fst_neigh.push_back(*n_it);
			}
			neighbour_vector snd_neigh;
			for (auto n_it = _adj_list.cbegin(snd); n_it != _adj_list.cend(snd); n_it++) {
				if (*n_it == fst)
					continue;
				snd_neigh.push_back(*n_it);
			}
			// we rewrite all neighbours anyway no need to keep track of direct positions
			// get common neighbours
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
			
			// TODO: best way to perform permutation? std::shuffle then reassign? done here...
			const degree_t fst_sample_size = static_cast<degree_t>(fst_neigh.size() - common_neigh.size());
			// not necessarily needed
			const degree_t snd_sample_size = static_cast<degree_t>(snd_neigh.size() - common_neigh.size());
			std::shuffle(disjoint_neigh.begin(), disjoint_neigh.end(), Aux::Random::getURNG());
			// assign first fst_sample_size to fst and last snd_sample_size to snd
			const tradeid_t fst_next_trade = *(_trade_list.get_trades(fst));
		   	std::cout << "Next TradeID of fst: " << fst_next_trade << std::endl;	
			// if not existent then max value, and below compare goes in favor of partner, if partner
			// has no partner as well then their values are equal (max and equal)
			// and tiebreaking is applied
			for (degree_t counter = 0; counter < fst_sample_size; counter++) {
				const node_t swapped = disjoint_neigh[counter];
				std::cout << "New edge: fst " << fst << ", " << swapped << std::endl;
				// TODO: maybe inline and modularize this (tie-breaking/putting in right adj_list row)
				// TODO: maybe don't return iterator? have it hidden?
				if (*(_trade_list.get_trades(swapped)) < fst_next_trade) {
					std::cout << "Next trade of swapped " << swapped << " before fst " << fst << std::endl;
				} else {
					if (*(_trade_list.get_trades(swapped)) > fst_next_trade) {
						std::cout << "Next trade of fst " << fst << " before swapped " << swapped << std::endl;
					} else {
						// arbitrary tie-breaking
						std::cout << "Next trade same" << std::endl;
					}
				}
			}
			const tradeid_t snd_next_trade = *(_trade_list.get_trades(snd));
			for (degree_t counter = fst_sample_size; counter < fst_sample_size + snd_sample_size; counter++) {
				const node_t swapped = disjoint_neigh[counter];
				std::cout << "New edge: snd " << snd << ", " << swapped << std::endl;
			}

			trade_count++;
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
