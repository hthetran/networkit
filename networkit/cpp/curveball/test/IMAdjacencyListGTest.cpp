/**
 * @file
 * @brief Test cases for the IM Adjacency List
 * @author Hung Tran
 */

#include "IMAdjacencyListGTest.h"
#include "../IMAdjacencyList.h"

namespace CurveBall {

using degree_vector = std::vector<degree_t>;
using neighbour_it = std::vector<node_t>::const_iterator;

TEST_F(IMAdjacencyListGTest, container) {
	degree_vector degrees;
	degrees.push_back(3);
	degrees.push_back(2);
	degrees.push_back(1);
	degrees.push_back(1);
	degrees.push_back(1);

	const edgeid_t degree_count = 8;

	IMAdjacencyList adj_list(&degrees, degree_count);

	// Test beginning iterator for 0
	neighbour_it n_it = adj_list.getBegin(0);
	++n_it; 
	++n_it; 
	++n_it;
	ASSERT_EQ(*n_it, LISTROW_END);
	n_it++; 
	
	n_it++;
	n_it++;
	ASSERT_EQ(*n_it, LISTROW_END);
	n_it++;

	n_it++;
	ASSERT_EQ(*n_it, LISTROW_END);
	n_it++;

	// Test beginning iterator for 1
	n_it = adj_list.getBegin(1);
	++n_it;
	++n_it;
	ASSERT_EQ(*n_it, LISTROW_END);

	// Test ending iterator for 2
	n_it = adj_list.getBegin(2);
	neighbour_it e_it = adj_list.getEnd(2);
	ASSERT_EQ(n_it, e_it);
}

}