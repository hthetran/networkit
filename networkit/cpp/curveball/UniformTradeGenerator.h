/*
 * UniformTradeGenerator.h
 *
 *  Created on: Jul 11, 2017
 *      Author: Hung Tran
 */
#pragma once

#ifndef CB_UNIFORMTRADEGENERATOR_H
#define CB_UNIFORMTRADEGENERATOR_H

#include "Trade.h"

namespace CurveBall {
    class UniformTradeGenerator {
    public:
        using value_type = std::vector<TradeDescriptor>;

    public:
        UniformTradeGenerator(const int_t trade_num, const node_t num_nodes);

        const value_type& generate();
    };
}

#endif //CB_UNIFORMTRADEGENERATOR_H
