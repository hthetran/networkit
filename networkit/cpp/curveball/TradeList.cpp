/*
 * IMTradeList.cpp
 *
 *  Created on: Jul 11, 2017
 *      Author: Hung Tran
 */

#include "TradeList.h"
#include <numeric>

namespace CurveBall {

	using tradeid_vector = std::vector<tradeid_t>;
	using trade_t = TradeDescriptor;
	using trade_vector = std::vector<trade_t>;

	TradeList::TradeList(const node_t num_nodes)
			: _num_nodes(num_nodes)
	{ }

	void TradeList::initialize(const trade_vector& trades) {
		_trade_list.clear();
		_trade_list.resize(2 * trades.size() + _num_nodes);
		_offsets.clear();
		_offsets.resize(_num_nodes + 1);

		assert(_num_nodes > 0);
		assert(trades.size() > 0);

		std::vector<tradeid_t> trade_count(_num_nodes);

		// Push occurrences
		for (const trade_t trade : trades) {
			assert(trade.first >= 0);
			assert(trade.first < _num_nodes);
			assert(trade.second >= 0);
			assert(trade.second < _num_nodes);

			trade_count[trade.first]++;
			trade_count[trade.second]++;
		}

		// add missing +1 for sentinel
		trade_count[0]++;
		std::partial_sum(trade_count.cbegin(), trade_count.cend(), _offsets.begin() + 1, [&](const tradeid_t a, const tradeid_t b){
			return a + b + 1;
		});
		// add dummy
		_offsets[_num_nodes] = 2 * trades.size() + _num_nodes - 1;

		// set sentinels
		for (node_t node = 1; node < _num_nodes; node++) {
			_trade_list[_offsets[node] - 1] = TRADELIST_END;
		}
		// set last entry as sentinel
		_trade_list.back() = TRADELIST_END;

		std::fill(trade_count.begin(), trade_count.end(), 0);
		{
			tradeid_t trade_id = 0;
			for (const trade_t& trade : trades) {
				auto updateNode = [&] (const node_t node) {
					const node_t pos = _offsets[node] + trade_count[node];
					_trade_list[pos] = trade_id;
					trade_count[node]++;
				};

				updateNode(trade.first);
				updateNode(trade.second);
				trade_id++;
			}
		}
	}

	TradeList::TradeList(const trade_vector& trades, const node_t num_nodes)
			: _trade_list(2 * trades.size() + num_nodes)
			, _offsets(num_nodes + 1)
			, _num_nodes(num_nodes)
	{
		// Manuel: see above

		assert(num_nodes > 0);
		assert(trades.size() > 0);

		std::vector<tradeid_t> trade_count(num_nodes);

		// Push occurences
		for (const trade_t trade : trades) {
			assert(trade.first >= 0);
			assert(trade.first < num_nodes);
			assert(trade.second >= 0);
			assert(trade.second < num_nodes);

			trade_count[trade.first]++;
			trade_count[trade.second]++;
		}

		// add missing +1 for sentinel
		trade_count[0]++;
		std::partial_sum(trade_count.cbegin(), trade_count.cend(), _offsets.begin() + 1, [&](const tradeid_t a, const tradeid_t b){
			return a + b + 1;
		});
		// add dummy
		_offsets[num_nodes] = 2 * trades.size() + num_nodes - 1;

		// set sentinels
		for (node_t node = 1; node < _num_nodes; node++) {
			_trade_list[_offsets[node] - 1] = TRADELIST_END;
		}
		// set last entry as sentinel
		_trade_list.back() = TRADELIST_END;

		std::fill(trade_count.begin(), trade_count.end(), 0);
		{
			tradeid_t trade_id = 0;
			for (const trade_t& trade : trades) {
				auto updateNode = [&] (const node_t node) {
					const node_t pos = _offsets[node] + trade_count[node];
					_trade_list[pos] = trade_id;
					trade_count[node]++;
				};

				updateNode(trade.first);
				updateNode(trade.second);
				trade_id++;
			}
		}
	}

}
