/**
 * @file
 * @brief Test cases for the Curveball algorithm
 * @author Hung Tran
 */

#include "CurveballGTest.h"
#include "../Curveball.h"
#include "../../graph/Graph.h"
#include "../Trade.h"
#include "../../Globals.h"
#include "../../generators/ErdosRenyiGenerator.h"
#include "../../generators/HyperbolicGenerator.h"
#include "../UniformTradeGenerator.h"

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
}

TEST_F(CurveballGTest, testRunManyTradesMultiple) {
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

	trade_vector tvec2;
	tvec2.push_back(TradeDescriptor{2, 3});
	tvec2.push_back(TradeDescriptor{5, 4});
	tvec2.push_back(TradeDescriptor{0, 4});

	algo.run(tvec2);

	NetworKit::Graph tGOut = algo.getGraph();	
	ASSERT_TRUE(tGOut.hasEdge(6, 7));
	ASSERT_EQ(tGOut.numberOfEdges(), 9);
}

TEST_F(CurveballGTest, testManyRandomRunsErdosRenyi) {
	const node_t numNodes = 100;
	const tradeid_t numTrades = 20;
	const NetworKit::count numRuns = 10;
	const NetworKit::count numTradeRuns = 3;

	for (NetworKit::count run = 0; run < numRuns; run++) {
		std::cout << "At test " << run << std::endl;
		NetworKit::ErdosRenyiGenerator generator(numNodes, 0.8);
		NetworKit::Graph G = generator.generate();
		std::vector<degree_t> degrees;
		degrees.reserve(numNodes);
		G.forNodes([&](node_t u) {
			if (G.degree(u) > 0)
				degrees.push_back(G.degree(u));
			else {
				// Prevent isolated nodes
				G.addEdge(u, 0);
				degrees[0]++;
				degrees.push_back(1);
			}
		});

		Curveball algo(G);
		for (NetworKit::count tradeRun = 0; tradeRun < numTradeRuns; tradeRun++) {
			UniformTradeGenerator gen(numTrades, numNodes);
			algo.run(gen.generate());
		}

		NetworKit::Graph outG = algo.getGraph();
		outG.forNodes([&](node_t u){
			ASSERT_EQ(degrees[u], outG.degree(u));
		});
	}
}

TEST_F(CurveballGTest, testManyRandomRunsHyperbolic) {
	const node_t numNodes = 100;
	const tradeid_t numTrades = 20;
	const NetworKit::count numRuns = 10;
	const NetworKit::count numTradeRuns = 3;

	for (NetworKit::count run = 0; run < numRuns; run++) {
	std::cout << "At test " << run << std::endl;
	NetworKit::HyperbolicGenerator generator(numNodes, 2);
	NetworKit::Graph G = generator.generate();
	std::vector<degree_t> degrees;
	degrees.reserve(numNodes);
	G.forNodes([&](node_t u) {
	if (G.degree(u) > 0)
	degrees.push_back(G.degree(u));
	else {
	// Prevent isolated nodes
	G.addEdge(u, 0);
	degrees[0]++;
	degrees.push_back(1);
	}
	});

	Curveball algo(G);
	for (NetworKit::count tradeRun = 0; tradeRun < numTradeRuns; tradeRun++) {
	UniformTradeGenerator gen(numTrades, numNodes);
	algo.run(gen.generate());
	}

	NetworKit::Graph outG = algo.getGraph();
	outG.forNodes([&](node_t u){
	ASSERT_EQ(degrees[u], outG.degree(u));
	});
	}
}

}
