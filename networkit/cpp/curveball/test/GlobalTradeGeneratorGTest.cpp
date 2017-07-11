/*
 * GlobalTradeGeneratorGTest.cpp
 *
 *  Created on: Jul 11, 2017
 *      Author: Hung Tran
 */

#include "GlobalTradeGeneratorGTest.h"
#include "../GlobalTradeGenerator.h"

namespace CurveBall {
    TEST_F(GlobalTradeGeneratorGTest, testTradeNumber) {
        GlobalTradeGenerator genEven(1, 10);
        auto tradesEven = genEven.generate();
        ASSERT_EQ(tradesEven.size(), 5);

        GlobalTradeGenerator genOdd(1, 15);
        auto tradesOdd = genOdd.generate();
        ASSERT_EQ(tradesOdd.size(), 7);

        GlobalTradeGenerator genOddTimesFive(5, 15);
        auto tradesOddTimesFive = genOddTimesFive.generate();
        ASSERT_EQ(tradesOddTimesFive.size(), 35);
    }

}