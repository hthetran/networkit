/*
 * AutocorrelationAnalysis.h
 *
 *  Created on: Jul 24, 2017
 *	Author: Hung Tran
 */
#pragma once

#ifndef CB_AUTOCORRELATIONANALYSIS_H
#define CB_AUTOCORRELATIONANALYSIS_H

#include "../Globals.h"
#include "../../../stx-btree/include/stx/btree_map.h"
#include "../graph/Graph.h"
#include "defs.h"
#include <string>

namespace CurveBall {
	
	using edge_vector = std::vector<edge_t>;
	using bool_vector = std::vector<bool>;
	using value_type = std::pair<edge_t, bool_vector>;
	using value_type_vector = std::vector<value_type>;
	using value_type_it = std::vector<value_type>::iterator;
	// using tree_type = stx::btree_map<edge_t, bool_vector>;
	using tree_type = std::map<edge_t, bool_vector>;

	class AutocorrelationAnalysis {
	protected:
		const NetworKit::count _max_sample_size;
		NetworKit::count _curr_sample_size;
		tree_type edge_existence;

	public:
		AutocorrelationAnalysis(const NetworKit::count max_sample_size);

		void addSample(const NetworKit::Graph& G);

		void addSample(const edge_vector& edges);

		value_type_vector getEdgeExistences() const;
	};
}

#endif
