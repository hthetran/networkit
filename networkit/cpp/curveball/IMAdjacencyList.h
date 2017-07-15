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

namespace CurveBall {

class IMAdjacencyList {
public:
	using degree_vector = std::vector<degree_t>;
	using neighbour_vector = std::vector<node_t>;
	using pos_vector = std::vector<edgeid_t>;
	using pos_it = pos_vector::iterator;
	using neighbour_it = neighbour_vector::iterator;
	using cneighbour_it = neighbour_vector::const_iterator;

protected:
	degree_vector _offsets;
	neighbour_vector _neighbours;
	pos_vector _begin;
	pos_vector _end;
	

public:
	IMAdjacencyList() = default;
	
	// Receives the degree_vector to initialize
	// As trades permute neighbours the degrees don't change
	IMAdjacencyList(const degree_vector& degrees,
					const edgeid_t degree_count); // edge count // graph? // tradelist

	void initialize(const degree_vector& degrees, const edgeid_t degree_count);

	// No Copy Constructor
	IMAdjacencyList(const IMAdjacencyList &) = delete;

	neighbour_it begin(const node_t node_id);

	neighbour_it end(const node_t node_id);

	cneighbour_it cbegin(const node_t node_id) const;
	
	cneighbour_it cend(const node_t node_id) const;

	void insert_neighbour(const node_t node_id, const node_t neighbour) {
		auto pos = begin(node_id) + _offsets[node_id];
		*pos = neighbour;

		_offsets[node_id]++;
		_end[node_id]++;
	}

	void reset_offsets() {
		for (auto offsets_it = _offsets.begin(); offsets_it != _offsets.end(); offsets_it++) {
			*offsets_it = 0;
		}

		return;
	}

	void reset_end() {
		_end = _begin;

		return;
	}
};

}

#endif /* CB_IMADJACENCYLIST_H_ */
