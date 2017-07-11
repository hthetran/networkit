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
    using rload_node_vector = std::vector< std::pair<uint64_t, node_t> >;
	using node_vector = std::vector<node_t>;

    // Behaviour we have here:
    // If num_nodes is even, nothing changes
    // If num_nodes is odd, we don't have a trading partner for the last node, therefore we omit the last node
    // probably will be amortised when run_length is wisely chosen
    GlobalTradeGenerator::GlobalTradeGenerator(const int_t run_length, const node_t num_nodes)
        : _num_nodes(num_nodes)
        , _run_length(run_length)
        , _trades_per_run((tradeid_t) (num_nodes / 2))
        //, _trades_out(run_length * _trades_per_run)
    { }

    const value_type& GlobalTradeGenerator::generate() {
        _trades_out.clear();

        for (int_t run = 0; run < _run_length; run++) {
            // TODO: Here: space-local aware implementation, for smaller instances it may be better to enforce direct accesses instead
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
                // TODO: may use direct indices, instead of push_back
                _trades_out.push_back(TradeDescriptor(fst, snd));
            }
        }

        return _trades_out;
    }
}
