/*
 * Curveball.h
 *
 *  Created on: Jul 12 2017
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

    class Curveball : public NetworKit::Algorithm {

    protected:
        const NetworKit::Graph& _G;
        const node_t _num_nodes;
	IMAdjacencyList _adj_list;
	IMTradeList _trade_list;
	bool hasRun;

        void load_from_graph(const trade_vector& trades, bool verbose = false);

        void restructure_graph(const trade_vector& trades, bool verbose = false);

	inline void update(node_t a, node_t b, bool verbose = false);

    public:
        Curveball(const NetworKit::Graph& G);

	void run() {
	    std::runtime_error("Invalid use of algorithm, provide trades!");

            return;
        };

        void run(const trade_vector& trades, const bool verbose = false);

        bool isParallel() const {
		return false;
        }

        NetworKit::Graph getGraph(const bool verbose = false);

	//NetworKit::Graph getMaterializedGraph(const bool verbose = false);
    };
}

#endif //CB_CURVEBALL_H
