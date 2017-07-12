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
        //trade_vector& _trades;
        const NetworKit::Graph& _G;
        //IMAdjacencyList _adj_list;
        //IMTradeList _trade_list;

        void load_from_graph();

        void restructure_graph();

    public:
        Curveball(const NetworKit::Graph& G);

        // doesnt override run() of superclass
        void run(trade_vector& trades);

        bool isParallel() const {
            return false;
        }

        NetworKit::Graph& getGraph() const;
    };
}

#endif //CB_CURVEBALL_H
