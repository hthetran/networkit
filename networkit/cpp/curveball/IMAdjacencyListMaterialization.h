/*
 * IMAdjacencyListMaterialization.h
 *
 *  Created on: Jul 18, 2017
 *      Author: Hung Tran
 */
#pragma once

#ifndef CB_IMADJACENCYLISTMATERIALIZATION_H
#define CB_IMADJACENCYLISTMATERIALIZATION_H

#include "defs.h"
#include "IMAdjacencyList.h"
#include "../Globals.h"
#include "../graph/Graph.h"

namespace NetworKit {

	class IMAdjacencyListMaterialization {

		protected:

		public:
			IMAdjacencyListMaterialization() = default;

			NetworKit::Graph materialize(const CurveBall::IMAdjacencyList& adj_list) const;
	};
}

#endif //CB_IMADJACENCYLISTMATERIALIZATION_H

