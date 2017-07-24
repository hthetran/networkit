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
		G.addEdge(0, 2);
		G.addEdge(3, 4);
		G.addEdge(2, 4);
		G.addEdge(1, 3);

		aa.addSample(G);

//		auto it = aa.begin();
//
//		std::cout << (*it).first << std::endl;
	}

}
