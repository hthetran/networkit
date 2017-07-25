/*
 * AutocorrelationAnalysisGTest.cpp
 *
 *  Created on: Jul 22, 2017
 *	Author: Hung Tran
 */

#include "AutocorrelationAnalysisGTest.h"
#include "../AutocorrelationAnalysis.h"
#include "../../graph/Graph.h"

namespace CurveBall {

	TEST_F(AutocorrelationAnalysisGTest, testGraphInput) {
		AutocorrelationAnalysis aa(3);
		
		NetworKit::Graph G(5);
		G.addEdge(0, 1);
		G.addEdge(3, 4);
		G.addEdge(2, 4);
		G.addEdge(1, 3);

		aa.addSample(G);

		auto edgeExistences = aa.getEdgeExistences();

		ASSERT_EQ(edgeExistences.size(), 4);

		// [0] -> edge(0, 1)
		ASSERT_TRUE((edgeExistences[0].second)[0]);
		ASSERT_FALSE((edgeExistences[0].second)[1]);
		ASSERT_FALSE((edgeExistences[0].second)[2]);

		G.removeEdge(0, 1);
		G.addEdge(0, 2);

		aa.addSample(G);

		auto edgeExistences2 = aa.getEdgeExistences();

		// [0] -> edge(0, 1)
		ASSERT_TRUE((edgeExistences2[0].second)[0]);
		ASSERT_FALSE((edgeExistences2[0].second)[1]);
		ASSERT_FALSE((edgeExistences2[0].second)[2]);
		// [1] -> edge(0, 2)
		ASSERT_FALSE((edgeExistences2[1].second)[0]);
		ASSERT_TRUE((edgeExistences2[1].second)[1]);
		ASSERT_FALSE((edgeExistences2[1].second)[2]);
		
		G.addEdge(0, 1);

		aa.addSample(G);

		auto edgeExistences3 = aa.getEdgeExistences();
		// [0] -> edge(0, 1)
		ASSERT_TRUE((edgeExistences3[0].second)[0]);
		ASSERT_FALSE((edgeExistences3[0].second)[1]);
		ASSERT_TRUE((edgeExistences3[0].second)[2]);
	}

}
