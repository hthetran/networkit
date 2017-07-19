/*
 * GlobalTradeGenerator.h
 *
 *  Created on: Jul 11, 2017
 *      Author: Hung Tran
 */
#pragma once

#ifndef CB_GLOBALTRADEGENERATOR_H
#define CB_GLOBALTRADEGENERATOR_H

#include "Trade.h"
#include "../Globals.h"

namespace CurveBall {
    class GlobalTradeGenerator {
    public:
        using value_type = std::vector<TradeDescriptor>;

    protected:
        const node_t _num_nodes;
        const tradeid_t _run_length;
        const tradeid_t _trades_per_run;
        value_type _trades_out;

    public:
        GlobalTradeGenerator(const tradeid_t run_length, const node_t num_nodes);

        const value_type& generate();
    };
}

#endif //CB_GLOBALTRADEGENERATOR_H
