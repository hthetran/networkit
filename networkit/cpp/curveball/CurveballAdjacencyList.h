/*
 * IMAdjacencyList.h
 *
 *  Created on: Jul 11, 2017
 *      Author: Hung Tran
 */

#pragma once

#ifndef CB_IMADJACENCYLIST_H_
#define CB_IMADJACENCYLIST_H_

#include "defs.h"

namespace CurveballImpl {

	class CurveballAdjacencyList {
	public:
		using degree_vector = std::vector<degree_t>;
		using neighbour_vector = std::vector<node_t>;
		using pos_vector = std::vector<edgeid_t>;
		using pos_it = pos_vector::iterator;
		using neighbour_it = neighbour_vector::iterator;
		using cneighbour_it = neighbour_vector::const_iterator;
		using nodepair_vector = std::vector< std::pair<node_t, node_t> >;

	protected:

		neighbour_vector _neighbours;
		degree_vector _offsets;
		pos_vector _begin;
		edgeid_t _degree_count;

	public:
		CurveballAdjacencyList() = default;

		// Receives the degree_vector to initialize
		// As trades permute neighbours the degrees don't change
		CurveballAdjacencyList(const degree_vector& degrees, const edgeid_t degree_count);

		void initialize(const degree_vector& degrees, const edgeid_t degree_count);

		void restructure();

		// No Copy Constructor
		CurveballAdjacencyList(const CurveballAdjacencyList &) = delete;

		neighbour_it begin(const node_t node_id);

		neighbour_it end(const node_t node_id);

		cneighbour_it cbegin(const node_t node_id) const;

		cneighbour_it cend(const node_t node_id) const;

		nodepair_vector getEdges() const;

		void insertNeighbour(const node_t node_id, const node_t neighbour) {
			auto pos = begin(node_id) + _offsets[node_id];

			assert(*pos != LISTROW_END);

			*pos = neighbour;

			_offsets[node_id]++;
		}

		node_t numberOfNodes() const {
			return static_cast<node_t>(_offsets.size());
		}

		node_t numberOfEdges() const {
			return static_cast<edgeid_t>(_degree_count);
		}

		void resetRow(const node_t node_id) {
			assert(node_id >= 0);
			assert(node_id < static_cast<node_t>(_offsets.size()));

			_offsets[node_id] = 0;

			return;
		}

		degree_t degreeAt(node_t node_id) const {
			assert(node_id < static_cast<node_t>(_offsets.size()));
			assert(node_id >= 0);

			return _begin[node_id + 1] - _begin[node_id] - 1;
		}
	};

}

#endif /* CB_IMADJACENCYLIST_H_ */
