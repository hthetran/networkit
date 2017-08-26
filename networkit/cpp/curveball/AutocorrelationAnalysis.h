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
//#include "../../../stx-btree/include/stx/btree_map.h"
#include "../graph/Graph.h"
#include "defs.h"
#include <string>

namespace CurveBall {

	//TODO: Change back to edge_t
	using nodepair_vector = std::vector<std::pair<node_t, node_t> >;
	using bool_vector = std::vector<bool>;
	using value_type = std::pair< std::pair<node_t, node_t>, bool_vector>;
	using value_type_vector = std::vector<value_type>;
	using value_type_it = std::vector<value_type>::iterator;
	// using tree_type = stx::btree_map<edge_t, bool_vector>;
	using tree_type = std::map< std::pair<node_t, node_t>, bool_vector>;
	using tree_type_it = tree_type::const_iterator;
	using indrate_vector = std::vector<double>;

	class AutocorrelationAnalysis {
	protected:
		const NetworKit::count _max_sample_size;
		NetworKit::count _curr_sample_size;
		tree_type edge_existence;
		tree_type_it pos;

	public:
		AutocorrelationAnalysis(const NetworKit::count max_sample_size);

		void addSample(const NetworKit::Graph& G);

		void addSample(const nodepair_vector& edges);

		/* Deprecated do not call this.
		 * Potentially uses up too much RAM.
		 */
		value_type_vector getEdgeExistences() const;

		void init();

		bool_vector get() const;

		void next();

		bool end() const;

		edgeid_t numberOfEdges() const;

		indrate_vector getIndependenceRate(const std::vector<NetworKit::count>& thinnings, const NetworKit::count run_length) const;
	};
}

#endif
