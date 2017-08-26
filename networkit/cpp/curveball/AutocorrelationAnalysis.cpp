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

	indrate_vector AutocorrelationAnalysis::getIndependenceRate(const std::vector<NetworKit::count>& thinnings, const NetworKit::count run_length) const {
		indrate_vector indrates;
		indrates.reserve(thinnings.size());

		// iterate over thinning values
		for (const auto thinning : thinnings) {
			// for each edge
			NetworKit::count independent_edges = 0;
			NetworKit::count none_edges = 0; // edges that have never appeared in the thinned time-series
			for (auto edgets_it = edge_existence.begin(); edgets_it != edge_existence.end(); edgets_it++) {
				// initialize counter for runlength and transition counting matrix
				NetworKit::count run = 1;
				NetworKit::count x[2][2] = { {0, 0}, {0, 0} };

				// time-series ts
				const bool_vector ts = (*edgets_it).second;
				auto ts_it = ts.begin();

				bool prev = ts[thinning];

				std::advance(ts_it, 2*thinning);

				// iterate over time-series in thinning steps
				for ( ; ts_it != ts.end(); std::advance(ts_it, thinning)) {
					// transition from 1 to 1
					if (prev && *ts_it)
						++x[1][1];
					// transition from 0 to 1
					else if (!prev && *ts_it)
						++x[0][1];
					// transition from 0 to 1
					else if (prev && !(*ts_it))
						++x[1][0];
					else
					// transition from 0 to 0
						++x[0][0];

					prev = *ts_it;

					++run;
					if (run == std::min(_curr_sample_size, run_length))
						break;
				}

				// edge never existed in thinned time-series
				if (x[0][0] == run_length - 1) {
					none_edges++;
					continue;
				}

				// calculate log linear estimate
				const NetworKit::count x_sum = x[0][0] + x[0][1] + x[1][0] + x[1][1];
				const double hat_x[2][2] = {
					{(x[0][0] + x[0][1])*(x[0][0] + x[1][0])/static_cast<double>(x_sum),
					 (x[0][0] + x[0][1])*(x[0][1] + x[1][1])/static_cast<double>(x_sum)
					},
					{(x[1][0] + x[1][1])*(x[0][0] + x[1][0])/static_cast<double>(x_sum),
					 (x[1][0] + x[1][1])*(x[0][1] + x[1][1])/static_cast<double>(x_sum)
					}
				};

				// calculate independence rate
				const double log_summand[2][2] = {
					{x[0][0] == 0 ? 0. : x[0][0]*log(hat_x[0][0]/x[0][0]),
					 x[0][1] == 0 ? 0. : x[0][1]*log(hat_x[0][1]/x[0][1])
					},
					{x[1][0] == 0 ? 0. : x[1][0]*log(hat_x[1][0]/x[1][0]),
					 x[1][1] == 0 ? 0. : x[1][1]*log(hat_x[1][1]/x[1][1])
					}
				};

				const double log_sum = log_summand[0][0] + log_summand[0][1] + log_summand[1][0] + log_summand[1][1];

				const double delta_BIC = (-2)*log_sum - log(_curr_sample_size - 1);

				if (delta_BIC < 0)
					independent_edges++;
			}

			indrates.push_back(independent_edges / static_cast<double>(edge_existence.size() - none_edges));
		}

		return indrates;
	}

}
