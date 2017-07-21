/*
 * EdgeVectorMaterialization.h
 *
 *  Created on: Jul 21, 2017
 *	Author: Hung Tran
 */
#pragma once

#ifndef CB_EDGEVECTORMATERIALIZATION_H
#define CB_EDGEVECTORMATERIALIZATION_H

#include "defs.h"
#include "../graph/Graph.h"

namespace NetworKit {

	using edge_vector = std::vector<edge_t>;
	using degree_vector = std::vector<degree_t>;

	class EdgeVectorMaterialization {
	
	protected:

	public:
		EdgeVectorMaterialization() = default;

		// if no degrees are given, GraphBuilder suffices
		NetworKit::Graph materialize(const edge_vector& edges, const degree_vector& degrees);
	};

}

#endif//CB_EDGEVECTORMATERIALIZATION_H

