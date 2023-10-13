// networkit-format
/*
 * DegreeIntervalSwitchingSeparated.hpp
 *
 *  Created on: 13.10.2023
 *      Author:  Hung Tran <htran@ae.cs.uni-frankfurt.de>
 */
#ifndef NETWORKIT_RANDOMIZATION_DEGREE_INTERVAL_SWITCHING_SEPARATED_HPP_
#define NETWORKIT_RANDOMIZATION_DEGREE_INTERVAL_SWITCHING_SEPARATED_HPP_

#include <networkit/randomization/DegreeIntervalSwitching.hpp>

namespace NetworKit {

class DegreeIntervalSwitchingSeparated : public DegreeIntervalSwitching {
public:
    /// Constructs an EdgeSwitch algorithm that contains a COPY of the input graph.
    explicit DegreeIntervalSwitchingSeparated(const Graph &G,
                                     const std::vector<std::pair<node, node>> &degreeIntervals,
                                     double numberOfSwitchesPerEdge = 10.0);

    ~DegreeIntervalSwitchingSeparated() override = default;

    void run();
};

}

#endif // NETWORKIT_RANDOMIZATION_DEGREE_INTERVAL_SWITCHING_SEPARATED_HPP_
