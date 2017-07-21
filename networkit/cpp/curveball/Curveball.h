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

    class Curveball : public NetworKit::Algorithm {

    protected:
        const NetworKit::Graph& _G;
        const node_t _num_nodes;
	IMAdjacencyList _adj_list;
	IMTradeList _trade_list;
		degree_t _max_degree;

        void load_from_graph(const trade_vector& trades, bool verbose = false);

        void restructure_graph(const trade_vector& trades, bool verbose = false);

		inline void update(const node_t a, const node_t b, bool verbose = false) {
			const tradeid_t ta = *(_trade_list.get_trades(a));
			const tradeid_t tb = *(_trade_list.get_trades(b));
			if (ta < tb) {
				if (verbose)
					std::cout << "node a: " << a << " [" << ta << "] before " << "node b: " << b << " [" << tb << "]" << std::endl;
				_adj_list.insert_neighbour(a, b);

				return;
			}

			if (ta > tb) {
				if (verbose)
					std::cout << "node a: " << a << " [" << ta << "] after " << "node b: " << b << " [" << tb << "]" << std::endl;

				_adj_list.insert_neighbour(b, a);

				return;
			}

			// ta == tb
			{
				if (verbose)
					std::cout << "node a: " << a << " [" << ta << "] again with " << "node b: " << b << " [" << tb << "]" << std::endl;

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

        NetworKit::Graph getGraph(const bool verbose = false) const;

	//NetworKit::Graph getMaterializedGraph(const bool verbose = false);
    };
}

#endif //CB_CURVEBALL_H
