/*
 * GlobalTradeGenerator.h
 *
 *  Created on: Jul 11, 2017
 *      Author: Hung Tran
 */
#pragma once

#ifndef CB_GLOBALTRADEGENERATOR_H
#define CB_GLOBALTRADEGENERATOR_H

#include "../Globals.h"
#include "defs.h"

namespace CurveballImpl {
	class GlobalTradeGenerator {
		public:
			using value_type = std::vector<TradeDescriptor>;

		protected:
			const node_t _num_nodes;
			const tradeid_t _run_length;
			const tradeid_t _trades_per_run;

		public:
			GlobalTradeGenerator(const tradeid_t run_length, const node_t num_nodes);

			value_type generate() const;
    };
}

#endif //CB_GLOBALTRADEGENERATOR_H
