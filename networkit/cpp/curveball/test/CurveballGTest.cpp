/*
 * CurveballGTest.h
 *
 *  Created on: Jul 12, 2017
 *	Author: Hung Tran
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
	ASSERT_TRUE(tGOut.hasEdge(7, 6));
	ASSERT_EQ(tGOut.numberOfEdges(), 9);

	/*
	NetworKit::Graph tGOutNew = algo.getMaterializedGraph();
	ASSERT_TRUE(tGOutNew.hasEdge(6, 7));
	tGOutNew.forNodes([&](node_t u) {
		ASSERT_EQ(tGOutNew.degree(u), tGOut.degree(u));
	});
	ASSERT_EQ(tGOutNew.numberOfEdges(), tGOut.numberOfEdges());
	*/
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
	const node_t numNodes = 10;
	const tradeid_t numTrades = 20;
	const NetworKit::count numRuns = 5;
	const NetworKit::count numTradeRuns = 5;
	const NetworKit::count pow = 4;
	for (NetworKit::count n = numNodes; n <= std::pow(numNodes, pow); n*=10) {
		std::cout << n << std::endl;
		for (NetworKit::count run = 0; run < numRuns; run++) {
			NetworKit::ErdosRenyiGenerator generator(n, 0.5);
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
				UniformTradeGenerator gen(numTrades, n);
				algo.run(gen.generate());
			}

			NetworKit::Graph outG = algo.getGraph();
			outG.forNodes([&](node_t u){
				ASSERT_EQ(degrees[u], outG.degree(u));
			});
		}
	}
}

TEST_F(CurveballGTest, testManyRandomRunsHyperbolic) {
	const node_t numNodes = 50;
	const tradeid_t numTrades = 10;
	const NetworKit::count numRuns = 10000;
	const NetworKit::count numTradeRuns = 3;

	for (NetworKit::count run = 0; run < numRuns; run++) {
		NetworKit::HyperbolicGenerator generator(numNodes, 2);
		NetworKit::Graph G = generator.generate();
		std::vector<degree_t> degrees;
		degrees.reserve(numNodes);
		bool zeroisolated = false;
		G.forNodes([&](node_t u) {
			if (G.degree(u) > 0)
				degrees.push_back(G.degree(u));
			else {
				// Prevent isolated nodes
				if (u != 0) {
					G.addEdge(u, 0);
					degrees[0]++;
					degrees.push_back(1);
				} else {
					degrees.push_back(0);
					zeroisolated = true;
				}
			}
		});
		if (zeroisolated) {
			for (node_t nodeid = 1; nodeid < numNodes; nodeid++) {
				if (!G.hasEdge(0, nodeid)) {
					G.addEdge(0, nodeid);
					degrees[0]++;
					degrees[nodeid]++; 
				} else
					continue;
			}
		}

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
