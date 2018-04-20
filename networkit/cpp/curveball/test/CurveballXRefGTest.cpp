/*
 * CurveballXRefGTest.h
 *
 *  Created on: Jul 12, 2017
 *	Author: Hung Tran, Manuel Penschuck
 */

#include "CurveballXRefGTest.h"
#include "../Curveball.h"
#include "../../Globals.h"
#include "../../generators/ErdosRenyiGenerator.h"
#include "../../generators/HyperbolicGenerator.h"
#include "../UniformTradeGenerator.h"
#include "../GlobalTradeGenerator.h"
#include "../../auxiliary/Random.h"

namespace CurveballImpl {

	using trade_vector = std::vector<TradeDescriptor>;

    void CurveballXRefGTest::prepareGraph(NetworKit::Graph& G) {
        auto numNodes = G.numberOfNodes();
        // Add edge to node 0, if isolated node
        // If 0 itself is isolated, add new node and connect 0 to it
        G.forNodes([&](node_t u) {
            if (!G.degree(u)) {
                if (u == 0) {
                    G.addEdge(0, numNodes - 1);
                } else {
                    G.addEdge(u, 0);
                }
            }
        });
    }

    void CurveballXRefGTest::checkWithGraph(const NetworKit::Graph& G, bool global) {
        auto numNodes = G.numberOfNodes();
        const tradeid_t numTrades = numNodes;
        const NetworKit::count numTradeRuns = 10;

        std::vector< UniformTradeGenerator::value_type > trades;
        if (global) {
            GlobalTradeGenerator gen(1, numNodes);
            for (unsigned int j = 0; j < numTradeRuns; j++)
                trades.emplace_back(gen.generate());
        } else {
            UniformTradeGenerator gen(numTrades, numNodes);
            for (unsigned int j = 0; j < numTradeRuns; j++)
                trades.emplace_back(gen.generate());
        }

        auto seed1 = Aux::Random::getSeed();
        auto seed2 = Aux::Random::getSeed();

        std::vector<NetworKit::Graph> graphs;
        for(unsigned i=0; i<2; i++) {
            Aux::Random::setSeed(seed1, false);
            NetworKit::Curveball algo(G, !!i);
            for(unsigned int j=0; j<numTradeRuns; j++)
                algo.run(trades[j]);
            graphs.push_back(algo.getGraph());
        }

        Aux::Random::setSeed(seed2, false);

        const auto& G0 = graphs[0];
        const auto& G1 = graphs[1];

        ASSERT_EQ(G0.numberOfNodes(), G.numberOfNodes());
        ASSERT_EQ(G0.numberOfNodes(), G1.numberOfNodes());

        G0.forNodes([&](node_t u) {
            auto n0 = G0.neighbors(u);
            auto n1 = G1.neighbors(u);
            std::sort(n0.begin(), n0.end());
            std::sort(n1.begin(), n1.end());
            ASSERT_EQ(n0, n1);
        });
    }


	TEST_P(CurveballXRefGTest, testCurveballErdosRenyi) {
        node_t numNodes = GetParam();

        Aux::Random::setSeed(1, false);

		NetworKit::ErdosRenyiGenerator generator(numNodes, 0.2);
		NetworKit::Graph G = generator.generate();

        this->prepareGraph(G);
        this->checkWithGraph(G, false);
        this->checkWithGraph(G, true);
	}

	TEST_P(CurveballXRefGTest, testCurveballHyperbolic) {
        node_t numNodes = GetParam();

        Aux::Random::setSeed(1, false);

		NetworKit::HyperbolicGenerator generator(numNodes);
		NetworKit::Graph G = generator.generate();

		this->prepareGraph(G);
        this->checkWithGraph(G, true);
        this->checkWithGraph(G, false);
	}


    INSTANTIATE_TEST_CASE_P(CurveballXRefGTestInst, CurveballXRefGTest,::testing::Values(100, 500, 1000));
}

