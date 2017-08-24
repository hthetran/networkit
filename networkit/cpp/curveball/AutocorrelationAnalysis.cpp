/*
 * AutocorrelationAnalysis.cpp
 *
 *  Created on: Jul 24, 2017
 *	Author: Hung Tran
 */

#include "AutocorrelationAnalysis.h"

namespace CurveBall {

	using nodepair_vector = std::vector< std::pair<node_t, node_t> >;
	using bool_vector = std::vector<bool>;
	using value_type = std::pair< std::pair<node_t, node_t>, bool_vector>;
	using value_type_it = std::vector<value_type>::iterator;
	using value_type_cit = std::vector<value_type>::const_iterator;
	using value_type_vector = std::vector<value_type>;
	using tree_type = std::map<std::pair<node_t, node_t>, std::vector<bool> >;
	using tree_type_cit = tree_type::const_iterator;
	using indrate_vector = std::vector<double>;

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

	void AutocorrelationAnalysis::addSample(const nodepair_vector& edges) {
		for (const auto edge : edges) {
			edge_t _edge = edge;
			if (_edge.first > _edge.second)
				std::swap(_edge.first, _edge.second);
			assert(_edge.first <= _edge.second);
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

	void AutocorrelationAnalysis::init() {
		pos = edge_existence.cbegin();
		return;
	}

	bool_vector AutocorrelationAnalysis::get() const {
		return (*pos).second;
	}

	void AutocorrelationAnalysis::next() {
		pos++;
	}

	bool AutocorrelationAnalysis::end() const {
		return pos == edge_existence.cend();
	}

	edgeid_t AutocorrelationAnalysis::numberOfEdges() const {
		return edge_existence.size();
	}

	indrate_vector AutocorrelationAnalysis::getIndependenceRate(std::vector<NetworKit::count>& thinnings, NetworKit::count runLength) const {
		// iterate over thinning values
		for (const auto thinning : thinnings) {
			// iterate over time series'
			// NetworKit::count run = 0;
			// NetworKit::count x[2][2] = { {0, 0}, {0, 0} };
			// for each edge
			for (const auto edgets_it = edge_existence.cbegin(); edgets_it != edge_existence.cend(); std::next(edgets_it)) {
				const auto ts = (*edgets_it).second;
				/*
				// transition from 1 to 1
				if (prev && *ts_it).second)
					++x[1][1];
				// transition from 0 to 1
				else if (!prev && *ts_it)
					++x[0][1];
				// transition from 0 to 1
				else if (prev && *ts_it)
					++x[1][0];
				else
				// transition from 0 to 0
					++x[0][0];

				++run;
				if (run == runLength)
					break;*/

				// calculate independence rate
			}

		}
	}
}
