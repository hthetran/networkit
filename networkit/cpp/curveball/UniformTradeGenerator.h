/*
 * UniformTradeGenerator.h
 *
 *  Created on: Jul 11, 2017
 *      Author: Hung Tran
 */
#pragma once

#ifndef CB_UNIFORMTRADEGENERATOR_H
#define CB_UNIFORMTRADEGENERATOR_H

#include "defs.h"

namespace CurveballImpl {
	class UniformTradeGenerator {
		public:
			using value_type = std::vector<TradeDescriptor>;

		protected:
			const node_t _num_nodes;
			const tradeid_t _trade_num;

		public:
			UniformTradeGenerator(const tradeid_t trade_num, const node_t num_nodes);

			value_type generate() const;
    };
}

#endif //CB_UNIFORMTRADEGENERATOR_H
