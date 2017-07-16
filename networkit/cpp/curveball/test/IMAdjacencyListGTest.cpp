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

TEST_F(IMAdjacencyListGTest, testContainerConstructor) {
	degree_vector degrees;
	degrees.push_back(3);
	degrees.push_back(2);
	degrees.push_back(1);
	degrees.push_back(1);
	degrees.push_back(1);

	const edgeid_t degree_sum = 8;

	IMAdjacencyList adj_list(degrees, degree_sum);

	// Test beginning iterator for 0
	neighbour_it n_it = adj_list.cbegin(0);
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
	n_it = adj_list.cbegin(1);
	++n_it;
	++n_it;
	ASSERT_EQ(*n_it, LISTROW_END);

	// Test ending iterator for 2
	n_it = adj_list.cbegin(2);
	neighbour_it e_it = adj_list.cend(2);
	ASSERT_EQ(n_it, e_it);
}

TEST_F(IMAdjacencyListGTest, testContainerInitializeMethod) {
	degree_vector degrees;
	degrees.push_back(3);
	degrees.push_back(2);
	degrees.push_back(1);
	degrees.push_back(1);
	degrees.push_back(1);

	const edgeid_t degree_sum = 8;

	IMAdjacencyList adj_list;
	adj_list.initialize(degrees, degree_sum);

	// Test beginning iterator for 0
	neighbour_it n_it = adj_list.cbegin(0);
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
	n_it = adj_list.cbegin(1);
	++n_it;
	++n_it;
	ASSERT_EQ(*n_it, LISTROW_END);

	// Test ending iterator for 2
	n_it = adj_list.cbegin(2);
	neighbour_it e_it = adj_list.cend(2);
	ASSERT_EQ(n_it, e_it);
}

TEST_F(IMAdjacencyListGTest, testInsertion) {
	degree_vector degrees;
	degrees.push_back(3);
	degrees.push_back(2);
	degrees.push_back(1);
	degrees.push_back(1);
	degrees.push_back(1);

	const edgeid_t degree_sum = 8;

	IMAdjacencyList adj_list(degrees, degree_sum);
	adj_list.insert_neighbour(0, 3);
	adj_list.insert_neighbour(0, 2);
	neighbour_it n_it = adj_list.cbegin(0);
	ASSERT_EQ(*n_it, 3);
	n_it++;
	ASSERT_EQ(*n_it, 2);
	n_it++;
	ASSERT_EQ(n_it, adj_list.cend(0));
}

TEST_F(IMAdjacencyListGTest, testReset) {
	degree_vector degrees;
	degrees.push_back(3);
	degrees.push_back(2);
	degrees.push_back(1);
	degrees.push_back(1);
	degrees.push_back(1);

	const edgeid_t degree_sum = 8;

	IMAdjacencyList adj_list(degrees, degree_sum);
	adj_list.insert_neighbour(0, 3);
	adj_list.insert_neighbour(0, 2);
	neighbour_it n_it = adj_list.cbegin(0);
	ASSERT_EQ(*n_it, 3);
	n_it++;
	ASSERT_EQ(*n_it, 2);
	n_it++;
	ASSERT_EQ(n_it, adj_list.cend(0));

	adj_list.reset_row(0);
	ASSERT_EQ(adj_list.cbegin(0), adj_list.cend(0));

	adj_list.insert_neighbour(0, 1);
	n_it = adj_list.cbegin(0);
	ASSERT_EQ(*n_it, 1);
	n_it++;
	ASSERT_EQ(n_it, adj_list.cend(0));

}

}
