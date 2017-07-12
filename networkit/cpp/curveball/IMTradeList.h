/**
 * @brief Implements the Trade List for each Node
 * @author Hung Tran
 */

#pragma once

#ifndef CB_IMTRADELIST_H_
#define CB_IMTRADELIST_H_

#include "defs.h"
#include "Trade.h"

namespace CurveBall {

class IMTradeList {
public:
	using edge_vector = std::vector<edge_t>;
	using offset_vector = std::vector<tradeid_t>;
	using tradeid_vector = std::vector<tradeid_t>;
	using trade = TradeDescriptor;
	using trade_vector = std::vector<trade>;
	using tradeid_it = tradeid_vector::const_iterator;

protected:
	tradeid_vector _trade_list;
	offset_vector _offsets;
	node_t _num_nodes;

public:
	IMTradeList() = default;

	// Receives the edge_vector to initialize 
	IMTradeList(const trade_vector& trades, const node_t num_nodes);

	// No Copy Constructor
	IMTradeList(const IMTradeList&) = delete;

	tradeid_it get_trades(const node_t nodeid) const {
		assert(nodeid >= 0);
		assert(nodeid < _num_nodes);
		return _trade_list.begin() + _offsets[nodeid];
	}

	void shift_offset(const node_t nodeid, const tradeid_t d) {
		assert(d >= 0);
		assert(nodeid >= 0);
		assert(nodeid < _num_nodes);
		if (nodeid < _num_nodes)
			assert(d <= _offsets[nodeid + 1] - _offsets[nodeid]);
		else 
			assert(d <= static_cast<tradeid_t>(_trade_list.size()) - _offsets[nodeid] - 1);

		_offsets[nodeid] += d;
	}

	// generator global, k times n / 2

	// generator uniform, nodenumber, count

};

}

#endif /*  CB_IMTRADELIST_H_ */
