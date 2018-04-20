/*
 * IMAdjacencyList.cpp
 *
 *  Created on: Jul 11, 2017
 *      Author: Hung Tran
 */

#include "CurveballAdjacencyList.h"

namespace CurveballImpl {

	using neighbour_vector = std::vector<node_t>;
	using degree_vector = std::vector<degree_t>;
	using degree_it = std::vector<degree_t>::const_iterator;
	using pos_vector = std::vector<edgeid_t>;
	using neighbour_it = neighbour_vector::iterator;
	using cneighbour_it = neighbour_vector::const_iterator;
	using nodepair_vector = std::vector< std::pair<node_t, node_t> >;

// public static constexpr degree_t LISTROW_END = std::numeric_limits<degree_t>::max();
/**
 * @brief Initialize method (when constructor can't be used)
 *
 */
	void CurveballAdjacencyList::initialize(const degree_vector& degrees,
	                                        const edgeid_t degree_count) {
		_neighbours.resize(degree_count + degrees.size() + 1);
		_offsets.resize(degrees.size());
		_begin.resize(degrees.size() + 1);
		_degree_count = degree_count;

		degree_t sum = 0;
		node_t node_id = 0;
		for (const degree_t node_degree : degrees) {
			_begin[node_id] = sum;

			assert(node_degree > 0);

			sum += node_degree;
			_neighbours[sum] = LISTROW_END;

			// shift after Sentinel
			sum += 1;
			node_id++;
		}
		_neighbours[sum] = LISTROW_END;
		_begin[degrees.size()] = sum;

		assert(sum == degree_count + degrees.size());
		assert(node_id == degrees.size());

		return;
	}

	void CurveballAdjacencyList::restructure() {
		std::fill(_offsets.begin(), _offsets.end(), 0);
		return;
	}

/**
 * @brief Constructor
 * @param degree_vector Pointer to a vector with node degrees
 * @param degree_count Sum of all degrees in degree_vector
 *
 * We add to each adjacency list entry a delimiter to mark the end
 */
	CurveballAdjacencyList::CurveballAdjacencyList(const degree_vector& degrees,
	                                               const edgeid_t degree_count)
			: _neighbours(degree_count + degrees.size() + 1)
			, _offsets(degrees.size())
			, _begin(degrees.size() + 1)
			, _degree_count(degree_count)
	{
		degree_t sum = 0;
		node_t node_id = 0;
		for (const degree_t node_degree : degrees) {
			_begin[node_id] = sum;

			// no isolated nodes allowed
			assert(node_degree > 0);

			sum += node_degree;
			_neighbours[sum] = LISTROW_END;

			sum += 1;
			node_id++;
		}
		_neighbours[sum] = LISTROW_END;
		_begin[degrees.size()] = sum;

		assert(sum == degree_count + degrees.size());
		assert(node_id == degrees.size());
	}

	neighbour_it CurveballAdjacencyList::begin(const node_t node_id) {
		return _neighbours.begin() + _begin[node_id];
	}

	neighbour_it CurveballAdjacencyList::end(const node_t node_id) {
		return _neighbours.begin() + _begin[node_id] + _offsets[node_id];
	}

	cneighbour_it CurveballAdjacencyList::cbegin(const node_t node_id) const {
		return _neighbours.cbegin() + _begin[node_id];
	}

	cneighbour_it CurveballAdjacencyList::cend(const node_t node_id) const {
		return _neighbours.cbegin() + _begin[node_id] + _offsets[node_id];
	}

	nodepair_vector CurveballAdjacencyList::getEdges() const {
		nodepair_vector edges;
		edges.reserve(_degree_count);
		for (node_t nodeid = 0; nodeid < static_cast<node_t>(_offsets.size()); nodeid++) {
			for (auto it = cbegin(nodeid); it != cend(nodeid); it++) {
				edges.push_back(std::make_pair(nodeid, *it));
			}
		}

		return edges;
	}

}
