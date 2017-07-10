/**
 * @file
 * @brief Test cases for the IM Trade List
 * @author Hung Tran
 */


#include "IMTradeListGTest.h"
#include "../IMTradeList.h"
#include "../Trade.h"

namespace CurveBall {

TEST_F(IMTradeListGTest, testMatchingNodeNumber) {
	std::vector<TradeDescriptor> trades;
	trades.push_back(TradeDescriptor(0, 1));
	trades.push_back(TradeDescriptor(0, 2));
	trades.push_back(TradeDescriptor(0, 3));
	trades.push_back(TradeDescriptor(0, 4));
	trades.push_back(TradeDescriptor(1, 4));

	const node_t num_nodes = 5;

	IMTradeList trade_list(&trades, num_nodes);
	
	auto trade_iter = trade_list.get_trades(0);
	ASSERT_EQ(*trade_iter, 0);
	trade_iter++;
	ASSERT_EQ(*trade_iter, 1);
	trade_iter++;
	ASSERT_EQ(*trade_iter, 2);
	trade_iter++;
	ASSERT_EQ(*trade_iter, 3);
	trade_iter++;
	ASSERT_EQ(*trade_iter, TRADELIST_END);
	
	trade_iter = trade_list.get_trades(1);
	ASSERT_EQ(*trade_iter, 0);
	trade_iter++;
	ASSERT_EQ(*trade_iter, 4);
	trade_iter++;
	ASSERT_EQ(*trade_iter, TRADELIST_END);
		
	trade_iter = trade_list.get_trades(4);
	ASSERT_EQ(*trade_iter, 3);
	trade_iter++;
	ASSERT_EQ(*trade_iter, 4);
	trade_iter++;
	ASSERT_EQ(*trade_iter, TRADELIST_END);
}

TEST_F(IMTradeListGTest, testNodesAtEndNoTrades) {
	std::vector<TradeDescriptor> trades;
	trades.push_back(TradeDescriptor(0, 1));
	trades.push_back(TradeDescriptor(0, 2));
	trades.push_back(TradeDescriptor(0, 3));
	trades.push_back(TradeDescriptor(0, 4));
	trades.push_back(TradeDescriptor(1, 4));

	const node_t num_nodes = 10;

	IMTradeList trade_list(&trades, num_nodes);

	auto trade_iter = trade_list.get_trades(5);
	ASSERT_EQ(*trade_iter, TRADELIST_END);

	trade_iter = trade_list.get_trades(8);
	ASSERT_EQ(*trade_iter, TRADELIST_END);

	trade_iter = trade_list.get_trades(9);
	ASSERT_EQ(*trade_iter, TRADELIST_END);
}

TEST_F(IMTradeListGTest, testNodesAtBeginningNoTrades) {
	std::vector<TradeDescriptor> trades;
	trades.push_back(TradeDescriptor(3, 4));
	trades.push_back(TradeDescriptor(3, 5));
	trades.push_back(TradeDescriptor(3, 6));
	trades.push_back(TradeDescriptor(3, 7));
	trades.push_back(TradeDescriptor(4, 7));

	const node_t num_nodes = 10;

	IMTradeList trade_list(&trades, num_nodes);

	auto trade_iter = trade_list.get_trades(0);
	ASSERT_EQ(*trade_iter, TRADELIST_END);

	trade_iter = trade_list.get_trades(1);
	ASSERT_EQ(*trade_iter, TRADELIST_END);

	trade_iter = trade_list.get_trades(2);
	ASSERT_EQ(*trade_iter, TRADELIST_END);
}

TEST_F(IMTradeListGTest, testNodesInMiddleNoTrades) {
	// 5, 6, 7 have no trades
	std::vector<TradeDescriptor> trades;
	trades.push_back(TradeDescriptor(3, 4));
	trades.push_back(TradeDescriptor(3, 8));
	trades.push_back(TradeDescriptor(3, 9));
	trades.push_back(TradeDescriptor(3, 10));
	trades.push_back(TradeDescriptor(4, 10));

	const node_t num_nodes = 12;

	IMTradeList trade_list(&trades, num_nodes);

	auto trade_iter = trade_list.get_trades(0);
	ASSERT_EQ(*trade_iter, TRADELIST_END);

	trade_iter = trade_list.get_trades(3);
	ASSERT_EQ(*trade_iter, 0);
	trade_iter++;
	trade_iter++;
	trade_iter++;
	ASSERT_EQ(*trade_iter, 3);
	trade_iter++;
	ASSERT_EQ(*trade_iter, TRADELIST_END);

	trade_iter = trade_list.get_trades(5);
	ASSERT_EQ(*trade_iter, TRADELIST_END);

	trade_iter = trade_list.get_trades(6);
	ASSERT_EQ(*trade_iter, TRADELIST_END);

	trade_iter = trade_list.get_trades(7);
	ASSERT_EQ(*trade_iter, TRADELIST_END);

	trade_iter = trade_list.get_trades(8);
	ASSERT_EQ(*trade_iter, 1);
	trade_iter++;
	ASSERT_EQ(*trade_iter, TRADELIST_END);
}

}