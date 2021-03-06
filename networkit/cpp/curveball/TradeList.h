/**
 * IMTradeList.h
 *
 *  Created on: Jul 18, 2017
 *      Author: Hung Tran
 */

#pragma once

#ifndef CB_IMTRADELIST_H_
#define CB_IMTRADELIST_H_

#include "defs.h"

namespace CurveballImpl {

	class TradeList {
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
		const node_t _num_nodes;

	public:
		TradeList(const node_t num_nodes);

		// Receives the edge_vector to initialize
		TradeList(const trade_vector& trades, const node_t num_nodes);

		// Initialize method
		void initialize(const trade_vector& trades);

		// No Copy Constructor
		TradeList(const TradeList&) = delete;

		tradeid_it getTrades(const node_t nodeid) const {
			assert(nodeid >= 0);
			assert(nodeid < _num_nodes);

			return _trade_list.begin() + _offsets[nodeid];
		}

		void incrementOffset(const node_t nodeid) {
			assert(nodeid >= 0);
			assert(nodeid < _num_nodes);
			assert(1 <= _offsets[nodeid + 1] - _offsets[nodeid]);

			_offsets[nodeid]++;
		}

		node_t numberOfNodes() const {
			return _num_nodes;
		}
	};

}

#endif /*  CB_IMTRADELIST_H_ */
