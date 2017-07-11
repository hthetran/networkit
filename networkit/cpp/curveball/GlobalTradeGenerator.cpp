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
            rload_node_vector node_payload;
            for (node_t node_id = 0; node_id < 2 * _trades_per_run; node_id++) {
                node_payload.push_back( std::pair<uint64_t, node_t>((uint64_t) Aux::Random::integer(), node_id) );
            }
            assert(node_payload.size() == static_cast<std::size_t>(2 * _trades_per_run));

            std::sort(node_payload.begin(), node_payload.end());

            auto rand_node_it = node_payload.cbegin();
            for (tradeid_t t_id = 0; t_id < _trades_per_run; t_id++) {
                assert(rand_node_it != node_payload.cend());

                const node_t fst = (*rand_node_it).second;
                rand_node_it++;
                const node_t snd = (*rand_node_it).second;
                rand_node_it++;
                // TODO: may use direct indices, instead of push_back
                _trades_out.push_back(TradeDescriptor(fst, snd));
            }
        }

        return _trades_out;
    }
}