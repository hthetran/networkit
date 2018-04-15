/**
 * defs.h
 *
 *  Created on: Jul 10, 2017
 *      Author: Hung Tran
 */

#pragma once

#ifndef DEFS_H_
#define DEFS_H_

#include "../Globals.h"
#include <assert.h> // why is this not included in networkit?
#include <vector> // this too
#include <algorithm> // this too
#include <cstdint>
#include <limits>
#include <ostream>

namespace CurveballImpl {

/**
 * @typedef node_t
 * @brief	Type for every node id
 */
using node_t = NetworKit::node;
constexpr node_t INVALID_NODE = std::numeric_limits<node_t>::max();

/**
 * @typedef TradeDescripter
 * @brief	Type for trades of two nodes
 */
using TradeDescriptor = std::pair<node_t, node_t>;

/**
 * @typedef tradeid_t
 * @brief	Type for every trade id
 */
using tradeid_t = NetworKit::node;
constexpr tradeid_t TRADELIST_END = std::numeric_limits<tradeid_t>::max();

/**
 * @typedef degree_t
 * @brief	Type for node degrees
 */
using degree_t = NetworKit::node;
constexpr degree_t LISTROW_END = std::numeric_limits<degree_t>::max();

/**
 * @typedef edgeid_t
 * @brief	Type for addressing edges
 */
using edgeid_t = NetworKit::node;

/**
 * @brief	Type for every (un)directed edge
 */
struct edge_t : public std::pair<node_t, node_t> {
	edge_t() : std::pair<node_t, node_t>() {}

	edge_t(const std::pair <node_t, node_t> &edge) : std::pair<node_t, node_t>(
		edge) {}

	edge_t(const node_t &v1, const node_t &v2) : std::pair<node_t, node_t>(v1,
																		   v2) {}

	static edge_t invalid() {
		return edge_t(INVALID_NODE, INVALID_NODE);
	}

	void normalize() {
		if (first > second)
			std::swap(first, second);
	}

	bool is_invalid() const {
		return (*this == invalid());
	}
};

struct depchain_msg {
	node_t node;
	tradeid_t next_trade;

	depchain_msg() {}

	depchain_msg(node_t node, tradeid_t next_trade) : node(node),
													  next_trade(next_trade) {}

	depchain_msg(const depchain_msg &) = default;

	bool operator<(const depchain_msg &o) const {
		return node < o.node;
	}
};

using trade_vector = std::vector<TradeDescriptor>;
using nodepair_vector = std::vector <std::pair<node_t, node_t>>;

} // namespace CurveballImpl

namespace std {
template<>
class numeric_limits<CurveballImpl::edge_t> {
public:
	static CurveballImpl::edge_t min() {
		return {numeric_limits<CurveballImpl::node_t>::min(), numeric_limits<CurveballImpl::node_t>::min()};
	};

	static CurveballImpl::edge_t max() {
		return {numeric_limits<CurveballImpl::node_t>::max(), numeric_limits<CurveballImpl::node_t>::max()};
	};
};
}

inline std::ostream &operator<<(std::ostream &os, const CurveballImpl::edge_t &t) {
	os << "edge(" << t.first << "," << t.second << ")";
	return os;
}



#endif /* DEFS_H_ */
