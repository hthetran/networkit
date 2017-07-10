/**
 * @file
 * @brief Test cases for the Trade Datastructure
 * @author Hung Tran
 */

#include "TradeGTest.h"
#include "../Trade.h"

namespace CurveBall {

using trade = TradeDescriptor;

TEST_F(TradeGTest, constructor) {
	trade t1(0, 1);
	ASSERT_EQ(t1.fst(), 0);
	ASSERT_EQ(t1.snd(), 1);

	trade t2(1, 0);
	ASSERT_EQ(t2.fst(), 0);
	ASSERT_EQ(t2.snd(), 1);
}

}