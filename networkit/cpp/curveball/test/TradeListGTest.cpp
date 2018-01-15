/**
 * IMTradeListGTest.cpp
 *
 *  Created on: Jul 11, 2017
 *	Author: Hung Tran
 */

#include "TradeListGTest.h"
#include "../TradeList.h"
namespace CurveBall {

	TEST_F(TradeListGTest, testTradeListConstructor) {
		std::vector<TradeDescriptor> trades =
				{{1,2}, {1,3}, {1,4}, {1,5}, {2,5}, {5,8}};

		/*
		 * Expected Structure:
		 * Node     Trade-ids it partakes in
		 * 0:       END
		 * 1:       0, 1, 2, 3, END
		 * 2:       0, 4, END
		 * 3:       1, END
		 * 4:       2, END
		 * 5:       3, 4, 5, END
		 * 6:       END
		 * 7:       END
		 * 8:       5, END
		 * 9:       END
		 */

		// more nodes
		const node_t num_nodes = 10;

		TradeList trade_list(trades, num_nodes);

		auto trade_iter = trade_list.getTrades(0);
		ASSERT_EQ(*trade_iter, TRADELIST_END);

		trade_iter = trade_list.getTrades(1);
		ASSERT_EQ(*trade_iter, 0); trade_iter++;
		ASSERT_EQ(*trade_iter, 1); trade_iter++;
		ASSERT_EQ(*trade_iter, 2); trade_iter++;
		ASSERT_EQ(*trade_iter, 3); trade_iter++;
		ASSERT_EQ(*trade_iter, TRADELIST_END);

		trade_iter = trade_list.getTrades(2);
		ASSERT_EQ(*trade_iter, 0); trade_iter++;
		ASSERT_EQ(*trade_iter, 4); trade_iter++;
		ASSERT_EQ(*trade_iter, TRADELIST_END);

		trade_iter = trade_list.getTrades(3);
		ASSERT_EQ(*trade_iter, 1); trade_iter++;
		ASSERT_EQ(*trade_iter, TRADELIST_END);

		trade_iter = trade_list.getTrades(4);
		ASSERT_EQ(*trade_iter, 2); trade_iter++;
		ASSERT_EQ(*trade_iter, TRADELIST_END);

		trade_iter = trade_list.getTrades(5);
		ASSERT_EQ(*trade_iter, 3); trade_iter++;
		ASSERT_EQ(*trade_iter, 4); trade_iter++;
		ASSERT_EQ(*trade_iter, 5); trade_iter++;
		ASSERT_EQ(*trade_iter, TRADELIST_END);

		ASSERT_EQ(*(trade_list.getTrades(6)), TRADELIST_END);

		ASSERT_EQ(*(trade_list.getTrades(7)), TRADELIST_END);

		trade_iter = trade_list.getTrades(8);
		ASSERT_EQ(*trade_iter, 5); trade_iter++;
		ASSERT_EQ(*trade_iter, TRADELIST_END);

		ASSERT_EQ(*(trade_list.getTrades(9)), TRADELIST_END);
	}

	TEST_F(TradeListGTest, testTradeListInitialize) {
		std::vector<TradeDescriptor> trades =
				{{1,2}, {1,3}, {1,4}, {1,5}, {2,5}, {5,8}};

		/*
		 * Expected Structure:
		 * Node     Trade-ids it partakes in
		 * 0:       END
		 * 1:       0, 1, 2, 3, END
		 * 2:       0, 4, END
		 * 3:       1, END
		 * 4:       2, END
		 * 5:       3, 4, 5, END
		 * 6:       END
		 * 7:       END
		 * 8:       5, END
		 * 9:       END
		 */

		// more nodes
		const node_t num_nodes = 10;

		TradeList trade_list(num_nodes);
		trade_list.initialize(trades);

		auto trade_iter = trade_list.getTrades(0);
		ASSERT_EQ(*trade_iter, TRADELIST_END);

		trade_iter = trade_list.getTrades(1);
		ASSERT_EQ(*trade_iter, 0); trade_iter++;
		ASSERT_EQ(*trade_iter, 1); trade_iter++;
		ASSERT_EQ(*trade_iter, 2); trade_iter++;
		ASSERT_EQ(*trade_iter, 3); trade_iter++;
		ASSERT_EQ(*trade_iter, TRADELIST_END);

		trade_iter = trade_list.getTrades(2);
		ASSERT_EQ(*trade_iter, 0); trade_iter++;
		ASSERT_EQ(*trade_iter, 4); trade_iter++;
		ASSERT_EQ(*trade_iter, TRADELIST_END);

		trade_iter = trade_list.getTrades(3);
		ASSERT_EQ(*trade_iter, 1); trade_iter++;
		ASSERT_EQ(*trade_iter, TRADELIST_END);

		trade_iter = trade_list.getTrades(4);
		ASSERT_EQ(*trade_iter, 2); trade_iter++;
		ASSERT_EQ(*trade_iter, TRADELIST_END);

		trade_iter = trade_list.getTrades(5);
		ASSERT_EQ(*trade_iter, 3); trade_iter++;
		ASSERT_EQ(*trade_iter, 4); trade_iter++;
		ASSERT_EQ(*trade_iter, 5); trade_iter++;
		ASSERT_EQ(*trade_iter, TRADELIST_END);

		ASSERT_EQ(*(trade_list.getTrades(6)), TRADELIST_END);

		ASSERT_EQ(*(trade_list.getTrades(7)), TRADELIST_END);

		trade_iter = trade_list.getTrades(8);
		ASSERT_EQ(*trade_iter, 5); trade_iter++;
		ASSERT_EQ(*trade_iter, TRADELIST_END);

		ASSERT_EQ(*(trade_list.getTrades(9)), TRADELIST_END);
	}

}
