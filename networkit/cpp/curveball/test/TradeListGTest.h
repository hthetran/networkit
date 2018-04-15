/*
 * IMTradeListGTest.h
 *
 *  Created on: Jul 10, 2017
 *	Author: Hung Tran
 */

#ifndef CB_IMTRADELISTGTEST_H_
#define CB_IMTRADELISTGTEST_H_

#include <gtest/gtest.h>

namespace CurveballImpl {

    class TradeListGTest : public testing::Test {
    public:
        TradeListGTest() = default;
        virtual ~TradeListGTest() = default;
    };
}

#endif /* CB_IMTRADELISTGTEST_H_ */
