/*
 * UniformTradeGenerator.cpp
 *
 *  Created on: Jul 11, 2017
 *  	Author: Hung Tran
 */

#include "UniformTradeGenerator.h"
#include "../auxiliary/Random.h"

namespace CurveBall {
	using value_type = std::vector<TradeDescriptor>;
	using node_vector = std::vector<node_t>;

	UniformTradeGenerator::UniformTradeGenerator(const tradeid_t trade_num,
	                                             const node_t num_nodes)
		: _num_nodes(num_nodes)
		, _trade_num(trade_num)
	{
		assert(_num_nodes > 1);
	}

	value_type UniformTradeGenerator::generate() const {
		value_type _trades_out;
		_trades_out.clear();
		_trades_out.reserve(_trade_num);

		for (tradeid_t t_id = 0; t_id < _trade_num; t_id++) {
			const node_t fst = Aux::Random::integer(_num_nodes - 1);
			while (true) {
				const node_t snd = Aux::Random::integer(_num_nodes - 1);
				if (fst != snd) {
					_trades_out.push_back(TradeDescriptor{fst, snd});
					break;
				}
			}
		}

		return _trades_out;
	}
}
