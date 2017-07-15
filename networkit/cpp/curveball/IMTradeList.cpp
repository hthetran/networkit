/*
 * IMTradeList.cpp
 *
 *  Created on: Jul 11, 2017
 *      Author: Hung Tran
 */

#include "IMTradeList.h"
#include "Trade.h"

namespace CurveBall {

using trade_t = TradeDescriptor;
using trade_vector = std::vector<trade_t>;

void IMTradeList::initialize(const trade_vector& trades, const node_t num_nodes) {
	_trade_list.resize(2 * trades.size() + num_nodes);
	_offsets.resize(num_nodes);
	_num_nodes = num_nodes;

	assert(num_nodes > 0);
	assert(trades.size() > 0);

	std::vector<tradeid_t> trade_count(num_nodes);

	// Push occurences
	for (const trade_t trade : trades) {
		assert(trade.fst() >= 0);
		assert(trade.fst() < num_nodes);
		assert(trade.snd() >= 0);
		assert(trade.snd() < num_nodes);

		trade_count[trade.fst()]++;
		trade_count[trade.snd()]++;
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
		const node_t fst_node = trade.fst();

		const node_t fst_pos = _offsets[fst_node] + tmp_counter[fst_node];
		_trade_list[fst_pos] = trade_id;
		tmp_counter[fst_node]++;

		// process second node
		const node_t snd_node = trade.snd();

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
	assert(num_nodes > 0);
	assert(trades.size() > 0);

	std::vector<tradeid_t> trade_count(num_nodes);

	// Push occurences
	for (const trade_t trade : trades) {
		assert(trade.fst() >= 0);
		assert(trade.fst() < num_nodes);
		assert(trade.snd() >= 0);
		assert(trade.snd() < num_nodes);

		trade_count[trade.fst()]++;
		trade_count[trade.snd()]++;
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
		const node_t fst_node = trade.fst();

		const node_t fst_pos = _offsets[fst_node] + tmp_counter[fst_node];
		_trade_list[fst_pos] = trade_id;
		tmp_counter[fst_node]++;

		// process second node
		const node_t snd_node = trade.snd();

		const node_t snd_pos = _offsets[snd_node] + tmp_counter[snd_node];
		_trade_list[snd_pos] = trade_id;
		tmp_counter[snd_node]++;

		trade_id++;
	}
}

}
