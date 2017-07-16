/**
 * @file
 * @brief Test cases for the Curveball algorithm
 * @author Hung Tran
 */

#include "CurveballGTest.h"
#include "../Curveball.h"
#include "../../graph/Graph.h"
#include "../Trade.h"

namespace CurveBall {

using trade_vector = std::vector<TradeDescriptor>;

TEST_F(CurveballGTest, testRunSingleTrades) {
	NetworKit::Graph tG(8);
	tG.addEdge(0, 1);
	tG.addEdge(0, 4);
	tG.addEdge(1, 2);
	tG.addEdge(2, 3);
	tG.addEdge(3, 4);
	tG.addEdge(0, 3);
	tG.addEdge(1, 3);
	tG.addEdge(1, 5);
	tG.addEdge(6, 7);

	Curveball algo(tG);

	trade_vector tvec;
	tvec.push_back(TradeDescriptor{0, 1});

	algo.run(tvec);

	NetworKit::Graph tGOut = algo.getGraph();	
	ASSERT_TRUE(tGOut.hasEdge(6, 7));
	ASSERT_EQ(tGOut.numberOfEdges(), 9);

	/*NetworKit::Graph tGOut = algo.getGraph();
	tG.forEdges([&](node_t u, node_t v) {
		ASSERT_TRUE(tGOut.hasEdge(u, v));
	});

	tGOut.forEdges([&](node_t u, node_t v) {
		ASSERT_TRUE(tG.hasEdge(u, v));
	});*/
}

TEST_F(CurveballGTest, testRunManyTrades) {
	NetworKit::Graph tG(8);
	tG.addEdge(0, 1);
	tG.addEdge(0, 4);
	tG.addEdge(1, 2);
	tG.addEdge(2, 3);
	tG.addEdge(3, 4);
	tG.addEdge(0, 3);
	tG.addEdge(1, 3);
	tG.addEdge(1, 5);
	tG.addEdge(6, 7);

	Curveball algo(tG);

	trade_vector tvec;
	tvec.push_back(TradeDescriptor{0, 1});
	tvec.push_back(TradeDescriptor{2, 3});
	tvec.push_back(TradeDescriptor{4, 5});
	tvec.push_back(TradeDescriptor{0, 3});
	tvec.push_back(TradeDescriptor{1, 2});
	tvec.push_back(TradeDescriptor{1, 5});
	tvec.push_back(TradeDescriptor{3, 2});
	tvec.push_back(TradeDescriptor{4, 0});
	tvec.push_back(TradeDescriptor{4, 5});

	algo.run(tvec);

	NetworKit::Graph tGOut = algo.getGraph();	
	ASSERT_TRUE(tGOut.hasEdge(6, 7));
	ASSERT_EQ(tGOut.numberOfEdges(), 9);

	/*NetworKit::Graph tGOut = algo.getGraph();
	tG.forEdges([&](node_t u, node_t v) {
		ASSERT_TRUE(tGOut.hasEdge(u, v));
	});

	tGOut.forEdges([&](node_t u, node_t v) {
		ASSERT_TRUE(tG.hasEdge(u, v));
	});*/
}

}
