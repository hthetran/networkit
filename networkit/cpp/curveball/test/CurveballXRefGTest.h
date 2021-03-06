/*
 * CurveballXRefGTest.h
 *
 *  Created on: Jul 13, 2017
 *      Author: Hung Tran
 */

#ifndef CB_CURVEBALL_XREF_GTEST_H
#define CB_CURVEBALL_XREF_GTEST_H

#include <gtest/gtest.h>
#include "../../graph/Graph.h"
#include "../defs.h"

namespace CurveballImpl {

class CurveballXRefGTest : public testing::Test, public ::testing::WithParamInterface<node_t>  {
    public:
        CurveballXRefGTest() = default;
        virtual ~CurveballXRefGTest() = default;

    protected:
        void prepareGraph(NetworKit::Graph& G);
        void checkWithGraph(const NetworKit::Graph&, bool);
    };
}

#endif //CB_CURVEBALL_XREF_GTEST_H
