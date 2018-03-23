/*
 * CurveballXRefGTest.h
 *
 *  Created on: Jul 12, 2017
 *	Author: Hung Tran
 */

#include "CurveballXRefGTest.h"
#include "../Curveball.h"
#include "../../Globals.h"
#include "../../generators/ErdosRenyiGenerator.h"
#include "../../generators/HyperbolicGenerator.h"
#include "../UniformTradeGenerator.h"
#include "../../auxiliary/Random.h"

namespace CurveBall {

	using trade_vector = std::vector<TradeDescriptor>;

    void CurveballXRefGTest::checkWithGraph(NetworKit::Graph& G) {
        auto numNodes = G.numberOfNodes();
        const tradeid_t numTrades = numNodes;
        const NetworKit::count numTradeRuns = 10;

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

        UniformTradeGenerator gen(numTrades, numNodes);
        std::vector< UniformTradeGenerator::value_type > trades;
        for(unsigned int j=0; j<numTradeRuns; j++)
            trades.emplace_back( gen.generate() );

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

		NetworKit::ErdosRenyiGenerator generator(numNodes, 0.3);
		NetworKit::Graph G = generator.generate();

        this->checkWithGraph(G);
	}

	TEST_P(CurveballXRefGTest, testCurveballHyperbolic) {
		node_t numNodes = GetParam();

        Aux::Random::setSeed(1, false);

		NetworKit::HyperbolicGenerator generator(numNodes);
		NetworKit::Graph G = generator.generate();

        this->checkWithGraph(G);
	}


    INSTANTIATE_TEST_CASE_P(CurveballXRefGTestInst, CurveballXRefGTest,::testing::Values(100, 500, 1000));
}

