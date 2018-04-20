/*
 * CurveballTFP.h
 *
 *  Created on: Jul 12, 2017
 *      Author: Hung Tran, Manuel Penschuck
 */
#pragma once

#ifndef CB_CURVEBALLTFP_H
#define CB_CURVEBALLTFP_H

#include "defs.h"

#include "../base/Algorithm.h"
#include "../graph/Graph.h"

#include "CurveballBase.h"

#include <tlx/radixheap_pair.hpp>

#include "../auxiliary/SignalHandling.h"

namespace CurveballImpl {

	class CurveballTFP : public CurveballBase {
    public:
		CurveballTFP(const NetworKit::Graph& G);
        virtual ~CurveballTFP() override final = default;

		void run(const trade_vector& trades);

		edgeid_t getNumberOfAffectedEdges() const  override final {
            return _aff_edges;
        }

		NetworKit::Graph getGraph() const override final;

		nodepair_vector getEdges() const override final;

    protected:
        bool hasRun;
        nodepair_vector _edges;
        std::vector<tradeid_t> _trade_successor;

        static constexpr size_t Radix = 64;

        template<typename KeyT, typename DataT>
        using pq_t = tlx::radixheap_pair<KeyT, DataT, Radix>;
        pq_t<tradeid_t, depchain_msg> _cbpq;

        degree_t _max_degree; // only valid after load_from_graph
        edgeid_t _aff_edges; // affected half-edges

        // Prepare run of CurveballTFP
        void load_from_graph(const trade_vector& trades, Aux::SignalHandler& handler);
        void restructure_graph(const trade_vector& trades, Aux::SignalHandler& handler);

        template <typename GetEdges>
        void build_depchain(const trade_vector& trades, GetEdges get_edges, Aux::SignalHandler& handler);

        std::vector<depchain_msg> get_tradelist(const trade_vector&) const;

};
}

#endif //CB_CURVEBALLTFP_H
