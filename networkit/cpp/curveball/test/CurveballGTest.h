/*
 * CurveballGTest.h
 *
 *  Created on: Jul 13, 2017
 *      Author: Hung Tran
 */

#ifndef CB_CURVEBALLGTEST_H
#define CB_CURVEBALLGTEST_H

#include <gtest/gtest.h>

namespace CurveBall {

    class CurveballGTest : public testing::Test, public ::testing::WithParamInterface<bool>  {
    public:
        CurveballGTest() = default;
        virtual ~CurveballGTest() = default;
    };
}

#endif //CB_CURVEBALLGTEST_H
