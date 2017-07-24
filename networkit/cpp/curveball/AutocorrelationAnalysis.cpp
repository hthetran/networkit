/*
 * AutocorrelationAnalysis.cpp
 *
 *  Created on: Jul 24, 2017
 *	Author: Hung Tran
 */

#include "AutocorrelationAnalysis.h"

namespace CurveBall {
	
	using edge_vector = std::vector<edge_t>;
	using bool_vector = std::vector<bool>;
	using value_type = std::pair<edge_t, bool_vector>;
	using value_type_it = std::vector<value_type>::iterator;

	AutocorrelationAnalysis::AutocorrelationAnalysis(const NetworKit::count max_sample_size) 
		: _max_sample_size(max_sample_size)
		, _curr_sample_size(0)
	{

	}

	//TODO: make readable
	void AutocorrelationAnalysis::addSample(const NetworKit::Graph& G) {
		G.forEdges([&](node_t u, node_t v) {
			assert(u < v);
			auto edge_pos = edge_existence.insert(edge_t{u, v}, std::vector<bool>(_max_sample_size, false));
			((*(edge_pos.first)).second)[_curr_sample_size] = true;
		});

		_curr_sample_size++;

		return;
	}

	void AutocorrelationAnalysis::addSample(const edge_vector& edges) {
		for (const auto edge : edges) {
			assert(edge.first < edge.second);
			auto edge_pos = edge_existence.insert(edge, std::vector<bool>(_max_sample_size, false));
			((*(edge_pos.first)).second)[_curr_sample_size] = true;
		}

		_curr_sample_size++;

		return;
	}
/*
	value_type_it AutocorrelationAnalysis::begin() {
		return edge_existence.begin();
	}

	value_type_it AutocorrelationAnalysis::end() {
		return edge_existence.end();
	}*/
}
