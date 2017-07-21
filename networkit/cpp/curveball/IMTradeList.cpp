/*
 * IMTradeList.cpp
 *
 *  Created on: Jul 11, 2017
 *      Author: Hung Tran
 */

#include "IMTradeList.h"
#include "Trade.h"
#include <iostream>

namespace CurveBall {

using trade_t = TradeDescriptor;
using trade_vector = std::vector<trade_t>;

IMTradeList::IMTradeList(const node_t num_nodes) 
    : _num_nodes(num_nodes)
{ }

void IMTradeList::initialize(const trade_vector& trades) {
	_trade_list.clear();
	_trade_list.resize(2 * trades.size() + _num_nodes);
	_offsets.clear();
	_offsets.resize(_num_nodes);

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
	
	// calc prefix sums...
	// Manuel: Consider putting short-lived variables into scope:
	// {
	//   tradeid_t pre_sum = 0;
	//   for(..) {}
	// }
	auto count_it = trade_count.begin();
	tradeid_t pre_sum = 0;
	node_t curr_node = 1;


	// Manuel:
	//  - this is a for loop
	//  - prefix sum can be computed with std::partial_sum
	do {
		assert(curr_node < _num_nodes);
		_offsets[curr_node] = pre_sum + *count_it + 1;
		pre_sum = _offsets[curr_node];
		_trade_list[_offsets[curr_node] - 1] = TRADELIST_END;
		count_it++;
		curr_node++;
	} while (curr_node < _num_nodes);
	
	// set last entry as sentinel
	_trade_list.back() = TRADELIST_END;

	// Manuel: Why not reuse trade_count?
	std::vector<tradeid_t> tmp_counter(_num_nodes);

	tradeid_t trade_id = 0;
	for (const trade_t& trade : trades) {
		/* Manuel: Lambda?
		auto updateNode = [&] (const node_t node) {
		    const pos = _offsets[node] + tmp_counter[node];
		    _trade_list[pos] = trade_id;
		    tmp_counter[node]++;
		};

		updateNode(trade.first);
		updateNode(trade.second);
		trade_id++;
		*/


		// process first node
		const node_t fst_node = trade.first;


		// process second node
		const node_t snd_node = trade.second;

		const node_t snd_pos = _offsets[snd_node] + tmp_counter[snd_node];
		_trade_list[snd_pos] = trade_id;
		tmp_counter[snd_node]++;

		trade_id++;
	}
}

/**
 * Again use a sentinel.
 */
IMTradeList::IMTradeList(const trade_vector& trades, const node_t num_nodes)
	: _trade_list(2 * trades.size() + num_nodes)
	, _offsets(num_nodes)
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
	
	// calc prefix sums...
	auto count_it = trade_count.begin();
	tradeid_t pre_sum = 0;
	node_t curr_node = 1;

	do {
		assert(curr_node < num_nodes);
		_offsets[curr_node] = pre_sum + *count_it + 1;
		pre_sum = _offsets[curr_node];
		_trade_list[_offsets[curr_node] - 1] = TRADELIST_END;
		count_it++;
		curr_node++;
	} while (curr_node < num_nodes);
	
	// set last entry as sentinel
	_trade_list.back() = TRADELIST_END;

	std::vector<tradeid_t> tmp_counter(num_nodes);

	tradeid_t trade_id = 0;
	for (const trade_t trade : trades) {
		// process first node
		const node_t fst_node = trade.first;

		const node_t fst_pos = _offsets[fst_node] + tmp_counter[fst_node];
		_trade_list[fst_pos] = trade_id;
		tmp_counter[fst_node]++;

		// process second node
		const node_t snd_node = trade.second;

		const node_t snd_pos = _offsets[snd_node] + tmp_counter[snd_node];
		_trade_list[snd_pos] = trade_id;
		tmp_counter[snd_node]++;

		trade_id++;
	}
}

}
