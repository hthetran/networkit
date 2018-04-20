/*
 * IMAdjacencyListMaterialization.h
 *
 *  Created on: Jul 18, 2017
 *      Author: Hung Tran
 */
#pragma once

#ifndef CB_IMADJACENCYLISTMATERIALIZATION_H
#define CB_IMADJACENCYLISTMATERIALIZATION_H

#include "defs.h"
#include "CurveballAdjacencyList.h"
#include "../Globals.h"
#include "../graph/Graph.h"

namespace NetworKit {

	class CurveballMaterialization {

	protected:
		const CurveballImpl::CurveballAdjacencyList& _adj_list;

	public:
		CurveballMaterialization(const CurveballImpl::CurveballAdjacencyList& adj_list);

		NetworKit::Graph toGraph(bool autoCompleteEdges, bool parallel);

	protected:
		void toGraphDirectSwap(Graph &G);
		void toGraphParallel(Graph &G);
		void toGraphSequential(Graph &G);
	};
}

#endif //CB_IMADJACENCYLISTMATERIALIZATION_H

