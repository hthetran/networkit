/**
 * @file
 * @brief Test cases for the Trade Datastructure
 * @author Hung Tran
 */

#include "TradeGTest.h"
#include "../Trade.h"

namespace CurveBall {

using trade = TradeDescriptor;

TEST_F(TradeGTest, testConstructor) {
	trade t1(0, 1);
	ASSERT_EQ(t1.first, 0);
	ASSERT_EQ(t1.second, 1);

	trade t2(1, 0);
	ASSERT_EQ(t2.first, 1);
	ASSERT_EQ(t2.second, 0);
}

}
