#include "IMTradeList.h"
#include "Trade.h"

namespace CurveBall {

using trade = TradeDescriptor;
using trade_vector = std::vector<trade>;

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

	std::vector<std::pair<node_t, tradeid_t> > occures;

	// Push occurences
	tradeid_t trade_id = 0;
	for (auto t_it = trades.begin(); t_it != trades.end(); t_it++, trade_id++) {
		assert((*t_it).fst() >= 0);
		assert((*t_it).fst() < num_nodes);
		assert((*t_it).snd() >= 0);
		assert((*t_it).snd() < num_nodes);
		occures.push_back(std::pair<node_t, tradeid_t>((*t_it).fst(), trade_id));
		occures.push_back(std::pair<node_t, tradeid_t>((*t_it).snd(), trade_id));
	}

	// Sort occurences, to get sorted list of node -> trade correspondence
	std::sort(occures.begin(), occures.end());

	// Fill helper datastructure	
	auto occ_it = occures.begin();
	node_t nodeid = 0;
	node_t prev = INVALID_NODE;
	for (tradeid_t pos = 0; pos < static_cast<tradeid_t>(_trade_list.size()); pos++) {
		auto node_trade = *occ_it;
		// no more node_trade pairs or at node with no trades
		if ((occ_it == occures.end()) || (nodeid < node_trade.first)) {
			// if node with no trades, only then set offset
			// nodes with trades are handled below, and get their offset there already
			if (prev != nodeid)
				_offsets[nodeid] = pos;
			_trade_list[pos] = TRADELIST_END;
			prev = nodeid;
			nodeid++;
			continue;
		} 
		if (nodeid == node_trade.first) {
			if (prev != nodeid)
				_offsets[nodeid] = pos;
			_trade_list[pos] = node_trade.second;
			occ_it++;
		}

		prev = nodeid;
	}
}

}