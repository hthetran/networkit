/*
 * CurveballBase.h
 *
 *  Created on: Jul 12, 2017
 *      Author: Hung Tran, Manuel Penschuck
 */
#pragma once

#ifndef CB_CURVEBALLBASE_H
#define CB_CURVEBALLBASE_H

#include "../graph/Graph.h"
#include "defs.h"

namespace CurveBall {

	class CurveballBase {
	public:
		CurveballBase(const NetworKit::Graph& G)
			: _G(G), _num_nodes(G.numberOfNodes())
		{}

		virtual void run(const trade_vector& trades) = 0;

		virtual edgeid_t getNumberOfAffectedEdges() const = 0;

		virtual NetworKit::Graph getGraph() const = 0;

		virtual nodepair_vector getEdges() const = 0;

	protected:
		const NetworKit::Graph& _G;
		const node_t _num_nodes;

	};
}

#endif //CB_CURVEBALLBASE_H
