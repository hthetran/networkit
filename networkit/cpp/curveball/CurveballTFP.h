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
#include "../../../radix-heap/radix_heap.h"

namespace CurveBall {

	class CurveballTFP : public NetworKit::Algorithm {

	protected:
		const NetworKit::Graph& _G;
		const node_t _num_nodes;

        nodepair_vector _edges;
        std::vector<tradeid_t> _trade_successor;
        radix_heap::pair_radix_heap<tradeid_t, depchain_msg> _cbpq;

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
