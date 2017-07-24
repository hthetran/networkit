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

	class AutocorrelationAnalysis {
	protected:
		const NetworKit::count _max_sample_size;
		NetworKit::count _curr_sample_size;
		stx::btree_map< edge_t, std::vector<bool> > edge_existence;

	public:
		AutocorrelationAnalysis(const NetworKit::count max_sample_size);

		void addSample(const NetworKit::Graph& G);

		void addSample(const edge_vector& edges);

		void dump(std::string fileName);
	};
}

#endif
