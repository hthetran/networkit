/**
 * Trade.h
 *
 *  Created on: Jul 10, 2017
 *      Author: Hung Tran
 */

#pragma once

#ifndef CB_TRADE_H_
#define CB_TRADE_H_

#include "defs.h"

namespace  CurveBall {

// Manuel: Move to defs?
using TradeDescriptor = std::pair<node_t, node_t>;

/*struct TradeDescriptor : public std::pair<node_t, node_t> {
public:
	TradeDescriptor() : std::pair<node_t, node_t>() { }
	TradeDescriptor(const std::pair<node_t, node_t> & trade) : std::pair<node_t, node_t>(trade) {}
	TradeDescriptor(const node_t & v1, const node_t v2) : std::pair<node_t, node_t>(v1, v2) {}
};

inline std::ostream &operator<<(std::ostream &os, TradeDescriptor const &m) {
	return os << "{trade nodes " << m.first << " and " << m.second << "}";
}*/

}

#endif /* CB_TRADE_H_ */
