// networkit-format
/*
 * AutocorrelationAnalysis.hpp
 *
 *  Created on: 11.10.2023
 *      Author: Hung Tran <htran@ae.cs.uni-frankfurt.de>
 */

#ifndef NETWORKIT_RANDOMIZATION_AUTOCORRELATION_ANALYSIS_HPP_
#define NETWORKIT_RANDOMIZATION_AUTOCORRELATION_ANALYSIS_HPP_

#include <numeric>

#include <networkit/Globals.hpp>
#include <networkit/auxiliary/Random.hpp>
#include <networkit/base/Algorithm.hpp>
#include <networkit/graph/Graph.hpp>
#include <networkit/randomization/DegreeIntervalSwitching.hpp>

namespace NetworKit {

class AutocorrelationAnalysis : public Algorithm {
public:
    explicit AutocorrelationAnalysis(DegreeIntervalSwitching &algorithm,
                                     const std::vector<size_t> &thinnings_,
                                     size_t minSnapshots,
                                     const std::string &algorithmLabel,
                                     const std::string &graphLabel,
                                     const std::string &outputFilename,
                                     size_t switchesPerEdge,
                                     size_t maxSnapshots);

    ~AutocorrelationAnalysis() override = default;

    void run() override;

private:
    struct ThinningCounter {
        size_t numNonIndependent = 0;
        size_t numIndependent = 0;
        ThinningCounter() = default;

        void update(bool none, double deltaBIC) {
            numIndependent += (!none && deltaBIC < 0);
            numNonIndependent += (!none && deltaBIC >= 0);
        }
    };

    struct TransitionCounter {
        double x00;
        double x01;
        double x10;
        double x11;
        TransitionCounter() = default;
        TransitionCounter(double y00, double y01, double y10, double y11)
            : x00(y00), x01(y01), x10(y10), x11(y11) {}

        void update(bool p, bool n) {
            x00 = x00 + (!p && !n);
            x01 = x01 + (!p && n);
            x10 = x10 + (p && !n);
            x11 = x11 + (p && n);
        }

        [[nodiscard]] double sum() const { return x00 + x01 + x10 + x11; }

        [[nodiscard]] TransitionCounter compute_independent_model_prediction() const {
            const double sum_x = sum();
            return TransitionCounter{(x00 + x01) * (x00 + x10) / sum_x, (x00 + x01) * (x01 + x11) / sum_x,
                                     (x10 + x11) * (x00 + x10) / sum_x, (x10 + x11) * (x01 + x11) / sum_x};
        }

        [[nodiscard]] double computeDeltaBIC() const {
            const TransitionCounter x_I_hat = compute_independent_model_prediction();
            const double x00G2 = (x00 == 0. ? 0. : x00 * log(x_I_hat.x00 / x00));
            const double x01G2 = (x01 == 0. ? 0. : x01 * log(x_I_hat.x01 / x01));
            const double x10G2 = (x10 == 0. ? 0. : x10 * log(x_I_hat.x10 / x10));
            const double x11G2 = (x11 == 0. ? 0. : x11 * log(x_I_hat.x11 / x11));
            const double delta_BIC = (-2.) * (x00G2 + x01G2 + x10G2 + x11G2) - log(sum());
            return delta_BIC;
        }

        [[nodiscard]] bool isNone() const { return x00 == sum(); }
    };

    DegreeIntervalSwitching &algorithm;
    const node numNodes;
    const edgeid initialNumEdges;
    const std::vector<size_t> &thinnings;
    size_t minSnapshots;
    std::string algorithmLabel;
    std::string graphLabel;
    std::string outputFilename;
    size_t switchesPerEdge;
    size_t maxSnapshots;
    const edgeid numPossibleEdges;
    const size_t startingSeed;

    std::vector<size_t> snapshots;
    size_t minChainLength;
    std::vector<edgeid> tPrevSnapshots;
    std::vector<edgeid> tProcSnapshots;
    std::vector<std::vector<TransitionCounter>> tEdgeTransitions;
    std::vector<std::vector<bool>> tEdgeBits;

    template <typename T>
    constexpr inline void swap_if(bool cond, T &a, T &b) {
        const auto tmp = (a ^ b) * cond;
        a ^= tmp;
        b ^= tmp;
    }

    constexpr inline edgeid to_edge(node a, node b) {
        swap_if(a > b, a, b);
        return (static_cast<edgeid>(a) << 32) | b;
    }

    constexpr inline std::pair<node, node> to_nodes(edgeid e) {
        return {static_cast<node>(e >> 32), static_cast<node>(e & 0xFFFFFFFF)};
    }

    index get_index(edgeid e) {
        const auto uv = to_nodes(e);
        const auto u = uv.first;
        const auto v = uv.second;
        const auto d = numNodes;
        return (d * (d - 1) / 2) - (d - u) * ((d - u) - 1) / 2 + v - u - 1;
    }

    edgeid get_edge(index i) {
        const auto d = numNodes;
        const auto u = d - 2 - static_cast<node>(std::floor(std::sqrt(-8 * i + 4 * d * (d - 1) - 7) / 2. - 0.5));
        const auto v = i + u + 1 - d * (d - 1) / 2 + (d - u) * ((d - u) - 1) / 2;
        return to_edge(u, v);
    }

    edgeid get_next_edge(edgeid e) {
        const auto uv = to_nodes(e);
        const auto u = uv.first;
        const auto v = uv.second;
        if (v == numNodes - 1) {
            return to_edge(u + 1, u + 2);
        } else {
            return to_edge(u, v + 1);
        }
    }
};

}

#endif //NETWORKIT_RANDOMIZATION_AUTOCORRELATION_ANALYSIS_HPP_
