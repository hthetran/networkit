/*
 * GlobalTradeGenerator.cpp
 *
 *  Created on: Jul 11, 2017
 *      Author: Hung Tran
 */

#include "GlobalTradeGenerator.h"
#include "../auxiliary/Random.h"

namespace CurveBall {
	using value_type = std::vector<TradeDescriptor>;
	using node_vector = std::vector<node_t>;

	// Behaviour we have here:
	// If num_nodes is even, nothing changes
	// If num_nodes is odd, we don't have a trading partner for the last node, therefore we omit the last node
	// probably will be amortised when run_length is wisely chosen
	GlobalTradeGenerator::GlobalTradeGenerator(const tradeid_t run_length, const node_t num_nodes)
		: _num_nodes(num_nodes)
		  , _run_length(run_length)
		  , _trades_per_run(static_cast<tradeid_t>(num_nodes / 2))
	{ }

	const value_type GlobalTradeGenerator::generate() {
		value_type _trades_out;
		_trades_out.reserve(_run_length * _trades_per_run);

		for (NetworKit::count run = 0; run < _run_length; run++) {
			node_vector node_permutation;
			for (node_t node_id = 0; node_id < _num_nodes; node_id++) {
				node_permutation.push_back(node_id);
			}

			std::shuffle(node_permutation.begin(), node_permutation.end(), Aux::Random::getURNG());

			auto rand_node_it = node_permutation.cbegin();
			for (tradeid_t t_id = 0; t_id < _trades_per_run; t_id++) {
				assert(rand_node_it != node_permutation.cend());

				const node_t fst = *rand_node_it;
				rand_node_it++;
				const node_t snd = *rand_node_it;
				rand_node_it++;

				_trades_out.push_back(TradeDescriptor{fst, snd});
			}
		}

		return _trades_out;
	}
}
