/*
 * IMAdjacencyListGTest.cpp
 *
 *  Created on: Jul 10, 2017
 *	Author: Hung Tran
 */

#include "CurveballAdjacencyListGTest.h"
#include "../CurveballAdjacencyList.h"

namespace CurveBall {

	using degree_vector = std::vector<degree_t>;
	using neighbour_it = std::vector<node_t>::const_iterator;

	TEST_F(CurveballAdjacencyListGTest, testContainer) {
		degree_vector degrees = {3, 2, 1, 1, 1};

		const edgeid_t degree_sum = 8;

		// =========== Container ===========

		CurveballAdjacencyList adj_list(degrees, degree_sum);

		/*
		 * Expected Structure:
		 * | 0           | 1        | 2     | 3     | 4     |
		 * +-------------+----------+-------+-------+-------+
		 * | _, _, _, END| _, _, END| _, END| _, END| _, END|
		 */

		// Node 0
		neighbour_it n_it = adj_list.cbegin(0);
		++n_it; ++n_it; ++n_it;
		ASSERT_EQ(*n_it, LISTROW_END);
		n_it++;

		// Node 1
		n_it++; n_it++;
		ASSERT_EQ(*n_it, LISTROW_END);
		n_it++;

		// Node 2
		n_it++;
		ASSERT_EQ(*n_it, LISTROW_END);
		n_it++;

		// Node 3
		n_it++;
		ASSERT_EQ(*n_it, LISTROW_END);
		n_it++;

		// Node 4
		n_it++;
		ASSERT_EQ(*n_it, LISTROW_END);

		// =========== Insertion ===========

		adj_list.insertNeighbour(0, 3);
		adj_list.insertNeighbour(0, 2);

		/*
		 * Expected Structure:
		 * | 0           | 1        | 2     | 3     | 4     |
		 * +-------------+----------+-------+-------+-------+
		 * | 3, 2, _, END| _, _, END| _, END| _, END| _, END|
		 */

		n_it = adj_list.cbegin(0);
		ASSERT_EQ(*n_it, 3);
		n_it++;
		ASSERT_EQ(*n_it, 2);
		n_it++;
		ASSERT_EQ(n_it, adj_list.cend(0));

		// =========== Reset ===========

		adj_list.resetRow(0);
		ASSERT_EQ(adj_list.cbegin(0), adj_list.cend(0));
	}

	TEST_F(CurveballAdjacencyListGTest, testContainerByInitialize) {
		degree_vector degrees = {3, 2, 1, 1, 1};

		const edgeid_t degree_sum = 8;

		// =========== Container ===========

		CurveballAdjacencyList adj_list;
		adj_list.initialize(degrees, degree_sum);

		/*
		 * Expected Structure:
		 * | 0           | 1        | 2     | 3     | 4     |
		 * +-------------+----------+-------+-------+-------+
		 * | _, _, _, END| _, _, END| _, END| _, END| _, END|
		 */

		// Node 0
		neighbour_it n_it = adj_list.cbegin(0);
		++n_it; ++n_it; ++n_it;
		ASSERT_EQ(*n_it, LISTROW_END);
		n_it++;

		// Node 1
		n_it++; n_it++;
		ASSERT_EQ(*n_it, LISTROW_END);
		n_it++;

		// Node 2
		n_it++;
		ASSERT_EQ(*n_it, LISTROW_END);
		n_it++;

		// Node 3
		n_it++;
		ASSERT_EQ(*n_it, LISTROW_END);
		n_it++;

		// Node 4
		n_it++;
		ASSERT_EQ(*n_it, LISTROW_END);

		// =========== Insertion ===========

		adj_list.insertNeighbour(0, 3);
		adj_list.insertNeighbour(0, 2);

		/*
		 * Expected Structure:
		 * | 0           | 1        | 2     | 3     | 4     |
		 * +-------------+----------+-------+-------+-------+
		 * | 3, 2, _, END| _, _, END| _, END| _, END| _, END|
		 */

		n_it = adj_list.cbegin(0);
		ASSERT_EQ(*n_it, 3);
		n_it++;
		ASSERT_EQ(*n_it, 2);
		n_it++;
		ASSERT_EQ(n_it, adj_list.cend(0));

		// =========== Reset ===========

		adj_list.resetRow(0);
		ASSERT_EQ(adj_list.cbegin(0), adj_list.cend(0));
	}
}
