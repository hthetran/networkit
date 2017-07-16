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
        trade_vector _trades;
        const NetworKit::Graph& _G;
        const node_t _num_nodes;
		IMAdjacencyList _adj_list;
		IMTradeList _trade_list;
		bool hasRun;

        void load_from_graph(bool verbose = true);

        void restructure_graph();

    public:
        Curveball(const NetworKit::Graph& G);

		void run() {
			std::runtime_error("Invalid use of algorithm, provide trades!");
		};

        void run(const trade_vector& trades, bool verbose = true);

        bool isParallel() const {
            return false;
        }

		void update(node_t a, node_t b, bool verbose = true);

        NetworKit::Graph getGraph();
    };
}

#endif //CB_CURVEBALL_H
