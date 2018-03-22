/*
 * CurveballIM.h
 *
 *  Created on: Jul 12, 2017
 *      Author: Hung Tran
 */
#pragma once

#ifndef CB_CURVEBALLIM_H
#define CB_CURVEBALLIM_H

#include "../base/Algorithm.h"
#include "../graph/Graph.h"

#include "CurveballBase.h"
#include "CurveballAdjacencyList.h"
#include "TradeList.h"

namespace CurveBall {

	class CurveballIM : public CurveballBase {
	public:
		CurveballIM(const NetworKit::Graph& G);

		void run(const trade_vector& trades) override final;

		edgeid_t getNumberOfAffectedEdges() const override final;

		NetworKit::Graph getGraph() const override final;

		nodepair_vector getEdges() const override final;


    protected:
        bool hasRun;
        CurveballAdjacencyList _adj_list;
        TradeList _trade_list;
        degree_t _max_degree;
        edgeid_t _aff_edges; // affected half-edges

        void load_from_graph(const trade_vector& trades);

        void restructure_graph(const trade_vector& trades);

        inline void update(const node_t a, const node_t b) {
            const tradeid_t ta = *(_trade_list.getTrades(a));
            const tradeid_t tb = *(_trade_list.getTrades(b));
            if (ta < tb) {
                _adj_list.insertNeighbour(a, b);
                return;
            }

            if (ta > tb) {
                _adj_list.insertNeighbour(b, a);
                return;
            }
            // ta == tb
            {
                _adj_list.insertNeighbour(a, b);
            }
        }
	};
}

#endif //CB_CURVEBALLIM_H
