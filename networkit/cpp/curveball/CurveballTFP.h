/*
 * CurveballTFP.h
 *
 *  Created on: Jul 12, 2017
 *      Author: Hung Tran, Manuel Penschuck
 */
#pragma once

#ifndef CB_CURVEBALLTFP_H
#define CB_CURVEBALLTFP_H

#define USETLX


#include "defs.h"

#include "../base/Algorithm.h"
#include "../graph/Graph.h"

#ifdef USETLX
#include <tlx/radixheap_pair.hpp>
#else
#include "../../../radix-heap/radix_heap.h"
#endif

namespace CurveBall {

	class CurveballTFP : public NetworKit::Algorithm {

	protected:
		const NetworKit::Graph& _G;
		const node_t _num_nodes;

        nodepair_vector _edges;
        std::vector<tradeid_t> _trade_successor;

        template<typename KeyT, typename DataT>
#ifdef USETLX
        using pq_t = tlx::radixheap_pair<KeyT, DataT>;
#else
        using pq_t = radix_heap::pair_radix_heap<KeyT, DataT>;
#endif
        pq_t<tradeid_t, depchain_msg> _cbpq;

        degree_t _max_degree; // only valid after load_from_graph
        edgeid_t _aff_edges; // affected half-edges

        // Prepare run of CurveballTFP
        void load_from_graph(const trade_vector& trades);
        void restructure_graph(const trade_vector& trades);

        template <typename GetEdges>
        void build_depchain(const trade_vector& trades, GetEdges get_edges);

        std::vector<depchain_msg> get_tradelist(const trade_vector&) const;



    public:
		CurveballTFP(const NetworKit::Graph& G);

		void run() override {
			std::runtime_error("Invalid use of algorithm, provide trades!");
			return;
		};

		void run(const trade_vector& trades);

		bool isParallel() const override {
			return false;
		}

		edgeid_t getNumberOfAffectedEdges() const {
            return _aff_edges;
        }

		NetworKit::Graph getGraph() const;

		nodepair_vector getEdges() const;
	};
}

#endif //CB_CURVEBALLTFP_H
