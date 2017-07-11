/**
 * @file
 * @brief Trade Descriptor
 * @author Hung Tran
 */

#pragma once

#ifndef CB_TRADE_H_
#define CB_TRADE_H_

#include "defs.h"

namespace  CurveBall {

using tradeid_t = int32_t;

class TradeDescriptor {
	node_t _fst;
	node_t _snd;

public:
	TradeDescriptor();

	TradeDescriptor(node_t fst, node_t snd)
		: _fst(fst)
		, _snd(snd) 
	{
		assert(fst != snd);
	 	if (fst > snd)
			std::swap(_fst, _snd);
	}

	node_t fst() const { return _fst; }
	node_t snd() const { return _snd; }

	void normalize() {
		if (_fst > _snd)
			std::swap(_fst, _snd);
	}
};

inline std::ostream &operator<<(std::ostream &os, TradeDescriptor const &m) {
	return os << "{trade nodes " << m.fst() << " and " << m.snd() << "}";
}

}

#endif /* CB_TRADE_H_ */