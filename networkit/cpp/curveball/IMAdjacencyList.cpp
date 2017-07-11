/*
 * IMAdjacencyList.cpp
 *
 *  Created on: Jul 11, 2017
 *      Author: Hung Tran
 */

#include "IMAdjacencyList.h"

namespace CurveBall {

using neighbour_vector = std::vector<node_t>;
using degree_vector = std::vector<degree_t>;
using degree_it = std::vector<degree_t>::const_iterator;
using pos_vector = std::vector<edgeid_t>;
using neighbour_it = neighbour_vector::const_iterator;


// public static constexpr degree_t LISTROW_END = std::numeric_limits<degree_t>::max();

/**
 * @brief Constructor
 * @param degree_vector Pointer to a vector with node degrees
 * @param degree_count Sum of all degrees in degree_vector
 *
 * We add to each adjacency list entry a delimiter to mark the end
 */
IMAdjacencyList::IMAdjacencyList(const degree_vector& degrees, // remove pointer
								 const edgeid_t degree_count) 
	: _neighbours(degree_count + degrees.size())
	, _begin(degrees.size())
	, _end(degrees.size())
{
	degree_t sum = 0;
	node_t node_id = 0;
	for (degree_it iter = degrees.begin(); iter != degrees.end(); iter++, node_id++) {
		_begin[node_id] = sum;
		_end[node_id] = sum;

		// no isolated nodes allowed
		assert(*iter > 0);
		
		sum += *iter;
		_neighbours[sum] = LISTROW_END;

		sum += 1;
	}
	assert(sum == static_cast<degree_t>(degree_count + degrees.size()));
	assert(node_id == static_cast<degree_t>(degrees.size()));
}

neighbour_it IMAdjacencyList::cbegin(const node_t node_id) const {
	return (_neighbours.begin() + _begin[node_id]);
}

neighbour_it IMAdjacencyList::cend(const node_t node_id) const {
	return (_neighbours.begin() + _end[node_id]);
}

}