/*
 * IMAdjacencyListMaterialization.cpp
 *
 *  Created on: Jul 18, 2017
 *      Author: Hung Tran
 */
#include "IMAdjacencyListMaterialization.h"

namespace CurveBall {
    
    NetworKit::Graph IMAdjacencyListMaterialization::materialize(const IMAdjacencyList& adj_list) const {
        //TODO
        NetworKit::Graph G(adj_list.numberOfNodes());
        return NetworKit::Graph(5);
    }

}
