/*
 * UniformTradeGeneratorGTest.cpp
 *
 *  Created on: Jul 11, 2017
 *      Author: Hung Tran
 */

#include "UniformTradeGeneratorGTest.h"
#include "../UniformTradeGenerator.h"

namespace CurveBall {
    TEST_F(UniformTradeGeneratorGTest, testGeneration) {
        UniformTradeGenerator gen(15, 10);
        auto trades = gen.generate();
        ASSERT_EQ((uint64_t) trades.size(), 15);
		for (auto t_it = trades.cbegin(); t_it != trades.cend(); t_it++) {
			ASSERT_LE((*t_it).first, 9);
			ASSERT_GE((*t_it).first, 0);
			ASSERT_LE((*t_it).second, 9);
			ASSERT_GE((*t_it).second, 0);
		}
    }

}
