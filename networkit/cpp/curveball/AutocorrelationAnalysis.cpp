/*
 * AutocorrelationAnalysis.cpp
 *
 *  Created on: Jul 24, 2017
 *	Author: Hung Tran
 */

#include "AutocorrelationAnalysis.h"

namespace CurveBall {
	
	using edge_vector = std::vector<edge_t>;

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

	//TODO
	void AutocorrelationAnalysis::dump(std::string fileName) {
		return;
	}
}
