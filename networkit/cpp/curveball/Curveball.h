/*
 * CurveballIM.h
 *
 *  Created on: Jul 12, 2017
 *      Author: Hung Tran, Manuel Penschuck
 */
#pragma once

#ifndef CB_CURVEBALL_H
#define CB_CURVEBALL_H

#include <memory>
#include "../base/Algorithm.h"

#include "defs.h"
#include "CurveballIM.h"
#include "CurveballTFP.h"

namespace NetworKit {

	class Curveball : public Algorithm {
	public:
		Curveball(const NetworKit::Graph& G, bool boost = false) {
			if (boost) {
				_algo.reset(new CurveBall::CurveballTFP(G));
			} else {
				_algo.reset(new CurveBall::CurveballIM(G));
			}
		}

		void run() override {
			std::runtime_error("Invalid use of algorithm, provide trades!");
			return;
		};

		void run(const CurveBall::trade_vector& trades) {
			_algo->run(trades);
		}

		bool isParallel() const override {
			return false;
		}

        CurveBall::edgeid_t getNumberOfAffectedEdges() const {
			return _algo->getNumberOfAffectedEdges();
		}

		NetworKit::Graph getGraph() const {
			return _algo->getGraph();
		}

        CurveBall::nodepair_vector getEdges() const {
			return _algo->getEdges();
		}

	protected:
		std::unique_ptr<CurveBall::CurveballBase> _algo;
	};

}

#endif //CB_CURVEBALL_H
