/**
 * @brief Internal Memory Adjacency List Datastructure
 * @author Hung Tran
 */

#pragma once

#ifndef CB_IMADJACENCYLIST_H_
#define CB_IMADJACENCYLIST_H_

#include "defs.h"
#include "iostream"

namespace CurveBall {

class IMAdjacencyList {
public:
	using degree_vector = std::vector<degree_t>;
	using neighbour_vector = std::vector<node_t>;
	using pos_vector = std::vector<edgeid_t>;
	using pos_it = pos_vector::iterator;
	using neighbour_it = neighbour_vector::const_iterator;

protected:
	degree_vector _offset;
	neighbour_vector _neighbours;
	pos_vector _begin;
	pos_vector _end;
	

public:
	// Receives the degree_vector to initialize
	// As trades permute neighbours the degrees don't change
	IMAdjacencyList(const degree_vector& degrees,
					const edgeid_t degree_count); // edge count // graph? // tradelist

	// No Copy Constructor
	IMAdjacencyList(const IMAdjacencyList &) = delete;

	neighbour_it cbegin(const node_t node_id) const;
	
	neighbour_it cend(const node_t node_id) const;
};

}

#endif /* CB_IMADJACENCYLIST_H_ */