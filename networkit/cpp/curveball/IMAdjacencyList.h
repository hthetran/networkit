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
	degree_vector _degrees;
	edgeid_t _degree_count;	

public:
	IMAdjacencyList() = default;
	
	// Receives the degree_vector to initialize
	// As trades permute neighbours the degrees don't change
	IMAdjacencyList(const degree_vector& degrees, const edgeid_t degree_count);

	void initialize(const degree_vector& degrees, const edgeid_t degree_count);

	void restructure();

	// No Copy Constructor
	IMAdjacencyList(const IMAdjacencyList &) = delete;

	neighbour_it begin(const node_t node_id);

	neighbour_it end(const node_t node_id);

	cneighbour_it cbegin(const node_t node_id) const;
	
	cneighbour_it cend(const node_t node_id) const;

	void insert_neighbour(const node_t node_id, const node_t neighbour) {
		const auto pos = begin(node_id) + _offsets[node_id];

		if (*pos == LISTROW_END) {
			std::cout << "Tried to write into sentinel??? to: " << node_id << " with " << neighbour << std::endl;
			std::cout << "Look at current entries:" << std::endl;
			auto beg = begin(node_id);
			while (beg != end(node_id)) {
				std::cout << *beg << std::endl;
				beg++;
			}

			assert(*pos != LISTROW_END);
		}

		*pos = neighbour;

		_offsets[node_id]++;
		_end[node_id]++;
	}

        node_t numberOfNodes() const {
            return static_cast<node_t>(_offsets.size());
        }

	void reset_row(const node_t node_id) {
		_offsets[node_id] = 0;
		_end[node_id] = _begin[node_id];

		return;
	}
};

}

#endif /* CB_IMADJACENCYLIST_H_ */
