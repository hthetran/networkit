/*
 * Curveball.h
 *
 *  Created on: Jul 12, 2017
 *      Author: Hung Tran
 */
#pragma once

#ifndef CB_CURVEBALL_H
#define CB_CURVEBALL_H

#include "../base/Algorithm.h"
#include "../graph/Graph.h"
#include "Trade.h"
#include "IMAdjacencyList.h"
#include "IMTradeList.h"

namespace CurveBall {

    using trade_vector = std::vector<TradeDescriptor>;

	using nodepair_vector = std::vector< std::pair<node_t, node_t> >;

    class Curveball : public NetworKit::Algorithm {

    protected:
        const NetworKit::Graph& _G;
        const node_t _num_nodes;
	IMAdjacencyList _adj_list;
	IMTradeList _trade_list;
	degree_t _max_degree;
	edgeid_t _aff_edges; // affected half-edges

        void load_from_graph(const trade_vector& trades);

        void restructure_graph(const trade_vector& trades);

		inline void update(const node_t a, const node_t b) {
			const tradeid_t ta = *(_trade_list.get_trades(a));
			const tradeid_t tb = *(_trade_list.get_trades(b));
			if (ta < tb) {
				_adj_list.insert_neighbour(a, b);
				return;
			}

			if (ta > tb) {
				_adj_list.insert_neighbour(b, a);
				return;
			}
			// ta == tb
			{
				_adj_list.insert_neighbour(a, b);
			}
		}

    public:
        Curveball(const NetworKit::Graph& G);

	void run() override {
	    std::runtime_error("Invalid use of algorithm, provide trades!");

            return;
        };

        void run(const trade_vector& trades);

        bool isParallel() const override {
		return false;
        }

	edgeid_t getNumberOfAffectedEdges() const;

        NetworKit::Graph getGraph() const;
    
	nodepair_vector getEdges() const;
    };
}

#endif //CB_CURVEBALL_H
