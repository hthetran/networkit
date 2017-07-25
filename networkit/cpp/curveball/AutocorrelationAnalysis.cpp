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
	using value_type_cit = std::vector<value_type>::const_iterator;
	using value_type_vector = std::vector<value_type>;

	AutocorrelationAnalysis::AutocorrelationAnalysis(const NetworKit::count max_sample_size) 
		: _max_sample_size(max_sample_size)
		, _curr_sample_size(0)
	{

	}

	//TODO: make readable
	void AutocorrelationAnalysis::addSample(const NetworKit::Graph& G) {
		G.forEdges([&](node_t u, node_t v) {
			const node_t _u = std::min(u, v);
			const node_t _v = std::max(u, v);
			auto edge_pos = edge_existence.insert(std::make_pair(edge_t{_u, _v}, std::vector<bool>(_max_sample_size, false)));
			((*(edge_pos.first)).second)[_curr_sample_size] = true;
		});

		_curr_sample_size++;

		return;
	}

	void AutocorrelationAnalysis::addSample(const edge_vector& edges) {
		for (const auto edge : edges) {
			edge_t _edge = edge;
			_edge.normalize();
			auto edge_pos = edge_existence.insert(std::make_pair(_edge, std::vector<bool>(_max_sample_size, false)));
			((*(edge_pos.first)).second)[_curr_sample_size] = true;
		}

		_curr_sample_size++;

		return;
	}

	value_type_vector AutocorrelationAnalysis::getEdgeExistences() const {
		value_type_vector result;
		result.reserve(edge_existence.size());

		for (auto cit = edge_existence.cbegin(); cit != edge_existence.cend(); cit++) {
			result.push_back(*cit);
		}

		return result;
	}
}
