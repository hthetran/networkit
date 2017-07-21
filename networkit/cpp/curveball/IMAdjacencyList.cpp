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
using neighbour_it = neighbour_vector::iterator;
using cneighbour_it = neighbour_vector::const_iterator;


// public static constexpr degree_t LISTROW_END = std::numeric_limits<degree_t>::max();
/**
 * @brief Initialize method (when constructor can't be used)
 *
 */
void IMAdjacencyList::initialize(const degree_vector& degrees, const edgeid_t degree_count) {
	_offsets.resize(degrees.size());
	_neighbours.resize(degree_count + degrees.size());
	_begin.resize(degrees.size());
	_end.resize(degrees.size());
	_degrees = degrees;
	_degree_count = degree_count;

	degree_t sum = 0;
	node_t node_id = 0;
	for (const degree_t node_degree : degrees) {
		_begin[node_id] = sum;
		_end[node_id] = sum;

		assert(node_degree > 0);

		sum += node_degree;
		_neighbours[sum] = LISTROW_END;

		// shift after Sentinel
		sum += 1;
		node_id++;
	}
	assert(sum == degree_count + degrees.size());
	assert(node_id == degrees.size());

	return;
}

void IMAdjacencyList::restructure() {
	std::fill(_offsets.begin(), _offsets.end(), 0);
	_end = _begin;

	return;
}

/**
 * @brief Constructor
 * @param degree_vector Pointer to a vector with node degrees
 * @param degree_count Sum of all degrees in degree_vector
 *
 * We add to each adjacency list entry a delimiter to mark the end
 */
IMAdjacencyList::IMAdjacencyList(const degree_vector& degrees, // remove pointer
								 const edgeid_t degree_count) 
	: _offsets(degrees.size())
	, _neighbours(degree_count + degrees.size())
	, _begin(degrees.size())
	, _end(degrees.size())
{
	degree_t sum = 0;
	node_t node_id = 0;
	for (const degree_t node_degree : degrees) {
		_begin[node_id] = sum;
		_end[node_id] = sum;

		// no isolated nodes allowed
		assert(node_degree > 0);
		
		sum += node_degree;
		_neighbours[sum] = LISTROW_END;

		sum += 1;
		node_id++;
	}
	assert(sum == static_cast<degree_t>(degree_count + degrees.size()));
	assert(node_id == static_cast<degree_t>(degrees.size()));
}

neighbour_it IMAdjacencyList::begin(const node_t node_id) {
	return _neighbours.begin() + _begin[node_id];
}

neighbour_it IMAdjacencyList::end(const node_t node_id) {
	return _neighbours.begin() + _end[node_id];
}

cneighbour_it IMAdjacencyList::cbegin(const node_t node_id) const {
	return _neighbours.cbegin() + _begin[node_id];
}

cneighbour_it IMAdjacencyList::cend(const node_t node_id) const {
	return _neighbours.cbegin() + _end[node_id];
}

}
