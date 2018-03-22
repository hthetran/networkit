/*
 * CurveballGTest.h
 *
 *  Created on: Jul 12, 2017
 *	Author: Hung Tran
 */

#include "CurveballGTest.h"
#include "../Curveball.h"
#include "../../graph/Graph.h"
#include "../../Globals.h"
#include "../../generators/ErdosRenyiGenerator.h"
#include "../../generators/HyperbolicGenerator.h"
#include "../UniformTradeGenerator.h"
#include "../../auxiliary/Random.h"

namespace CurveBall {

	using trade_vector = std::vector<TradeDescriptor>;

	TEST_P(CurveballGTest, testCurveballErdosRenyi) {
        bool turbo = GetParam();

        Aux::Random::setSeed(1, false);

		node_t numNodes = 20;
		const tradeid_t numTrades = 5;
		const NetworKit::count numTradeRuns = 5;

		NetworKit::ErdosRenyiGenerator generator(numNodes, 0.3);
		NetworKit::Graph G = generator.generate();
		std::vector<degree_t> degrees(numNodes + 1);

		// Add edge to node 0, if isolated node
		// If 0 itself is isolated, add new node and connect 0 to it
		G.forNodes([&](node_t u) {
			if (G.degree(u) > 0)
				degrees[u] = G.degree(u);
			else {
				if (u == 0) {
					numNodes++;
					G.addEdge(0, numNodes - 1);
					degrees[0]++;
					degrees[numNodes - 1] = 1;
				} else {
					G.addEdge(u, 0);
					degrees[0]++;
					degrees[u] = 1;
				}
			}
		});


        NetworKit::Curveball algo(G, turbo);
        for (NetworKit::count tradeRun = 0; tradeRun < numTradeRuns; tradeRun++) {
            UniformTradeGenerator gen(numTrades, numNodes);
            algo.run(gen.generate());
        }

        // check degrees
        NetworKit::Graph outG = algo.getGraph();
        outG.forNodes([&](node_t u){
            ASSERT_EQ(degrees[u], outG.degree(u));
        });
	}

	TEST_P(CurveballGTest, testCurveballHyperbolic) {
        bool turbo = GetParam();

		node_t numNodes = 5000;
		const tradeid_t numTrades = 500;
		const NetworKit::count numTradeRuns = 10;

		NetworKit::HyperbolicGenerator generator(numNodes);
		NetworKit::Graph G = generator.generate();
		std::vector<degree_t> degrees(numNodes + 1);

		// Add edge to node 0, if isolated node
		// If 0 itself is isolated, add new node and connect 0 to it
		G.forNodes([&](node_t u) {
			if (G.degree(u) > 0)
				degrees[u] = G.degree(u);
			else {
				if (u == 0) {
					numNodes++;
					G.addEdge(0, numNodes - 1);
					degrees[0]++;
					degrees[numNodes - 1] = 1;
				} else {
					G.addEdge(u, 0);
					degrees[0]++;
					degrees[u] = 1;
				}
			}
		});

        NetworKit::Curveball algo(G, turbo);
        for (NetworKit::count tradeRun = 0; tradeRun < numTradeRuns; tradeRun++) {
            UniformTradeGenerator gen(numTrades, numNodes);
            algo.run(gen.generate());
        }

        // check degrees
        NetworKit::Graph outG = algo.getGraph();
        outG.forNodes([&](node_t u){
            ASSERT_EQ(degrees[u], outG.degree(u));
        });
	}


    INSTANTIATE_TEST_CASE_P(CurveballGTestInst, CurveballGTest,::testing::Bool());
}

