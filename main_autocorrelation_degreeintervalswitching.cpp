/*
 * main_autocorrelation_degreeintervalswitching.cpp
 *
 *  Created on: 13.10.2023
 *      Author:  Hung Tran <htran@ae.cs.uni-frankfurt.de>
 */

#include <tlx/cmdline_parser.hpp>
#include <networkit/graph/Graph.hpp>
#include <networkit/graph/GraphTools.hpp>
#include <networkit/generators/ErdosRenyiGenerator.hpp>
#include <networkit/generators/HyperbolicGenerator.hpp>
#include <networkit/randomization/AutocorrelationAnalysis.hpp>
#include <networkit/randomization/DegreeIntervalSwitchingSeparated.hpp>

NetworKit::Graph generate_graph(unsigned graph_type, unsigned num_nodes, double p, double avg_deg, double deg_exp) {
    switch (graph_type) {
    case 1:
    {
        NetworKit::ErdosRenyiGenerator generator(num_nodes, p);
        const NetworKit::Graph G = generator.generate();
        std::unordered_map<NetworKit::node, NetworKit::node> nodeIds = std::move(NetworKit::GraphTools::getContinuousNodeIds(G));
        NetworKit::Graph compactedG = NetworKit::GraphTools::getCompactedGraph(G, nodeIds);
        compactedG.sortEdges();
        compactedG.shrinkToFit();
        return compactedG;
    }
        break;
    case 2:
    {
        NetworKit::HyperbolicGenerator generator(num_nodes, avg_deg, deg_exp);
        const NetworKit::Graph G = generator.generate();
        std::unordered_map<NetworKit::node, NetworKit::node> nodeIds = std::move(NetworKit::GraphTools::getContinuousNodeIds(G));
        NetworKit::Graph compactedG = NetworKit::GraphTools::getCompactedGraph(G, nodeIds);
        compactedG.sortEdges();
        compactedG.shrinkToFit();
        return compactedG;
    }
        break;
    }
    return NetworKit::Graph();
}

int main(int argc, char *argv[]) {
    tlx::CmdlineParser cp;
    cp.set_description("Autocorrelation Analysis of the DegreeIntervalSwitching-Chain");

    // algorithm specific parameters
    unsigned algo = 0;
    cp.add_param_unsigned("algo", algo, "Algorithm; 1=DIS-SampleEdges, 2=DIS-SampleEdges-S, 3=DIS-SampleTuples, 4=DIS-SampleTuples-S");

    double id_prob = 1./3.;
    cp.add_double("idprob", id_prob, "Insertion/Deletion Probability");

    double hf_prob = 1./3.;
    cp.add_double("hfprob", hf_prob, "HingeFlip Probability");

    double es_prob = 1./3.;
    cp.add_double("esprob", es_prob, "HingeFlip Probability");

    bool separated = false;
    cp.add_flag("separated", separated, "Separate ID,HF,ES Operations?");

    // graph specific parameters
    unsigned graph_type = 0;
    cp.add_param_unsigned("graphtype", graph_type, "Graph Type; 1=Gilbert, 2=Hyperbolic");

    unsigned num_nodes = 0;
    cp.add_param_unsigned("nodes", num_nodes, "Number of Nodes");

    double p = 0;
    cp.add_double("p", p, "Gilbert: Probability of Edge Existence");

    double avg_deg = 0;
    cp.add_double("avgdeg", avg_deg, "Hyperbolic: Average Degree");

    double deg_exp = 0;
    cp.add_double("degexp", deg_exp, "Hyperbolic: Degree Exponent > 2");

    // autocorrelation analysis specific parameters
    unsigned runs = 0;
    cp.add_param_unsigned("runs", runs, "Runs");

    unsigned min_snapshots = 5000;
    cp.add_unsigned("minsnaps", min_snapshots, "Minimum Number of Snapshots per Thinning");

    unsigned max_snapshots = 5000;
    cp.add_unsigned("maxsnaps", max_snapshots, "Maximum Number of Snapshots per Thinning");

    unsigned switches_per_edge = 1;
    cp.add_unsigned("switchesperedge", switches_per_edge, "Switches per Edge");

    std::string algo_label;
    cp.add_param_string("algolabel", algo_label, "Algorithm Label");

    std::string graph_label;
    cp.add_param_string("graphlabel", graph_label, "Graph Label");

    std::string output_fn_prefix;
    cp.add_param_string("outputfnprefix", output_fn_prefix, "Output Filename Prefix");

    std::vector<std::string> thinnings_str;
    cp.add_param_stringlist("thinnings", thinnings_str, "Thinning Values e.g. --thinnings 1 2 3 4");

    // evaluate command line parser
    if (!cp.process(argc, argv)) {
        return -1;
    }

    std::cout << "# successfully processed command line arguments" << std::endl;

    std::vector<size_t> thinnings;
    thinnings.reserve(thinnings_str.size());
    for (auto& thinning_str : thinnings_str) {
        std::stringstream tss(thinning_str);
        size_t thinning;
        tss >> thinning;
        if (!tss) {
            return 0;
        }
        thinnings.push_back(thinning);
    }

    // run autocorrelation analysis
    for (auto i = 0; i < runs; ++i) {
        // generate graph
        const NetworKit::Graph G = std::move(generate_graph(graph_type, num_nodes, p, avg_deg, deg_exp));
        std::vector<std::pair<NetworKit::node, NetworKit::node>> degreeIntervals;
        degreeIntervals.reserve(G.numberOfNodes());
        G.forNodes([&](NetworKit::node u) {
          const auto deg = G.degree(u);
          degreeIntervals.emplace_back(deg, deg + 5);
        });

        std::unique_ptr<NetworKit::DegreeIntervalSwitching> dis_algo;
        switch (algo) {
        case 1:
            dis_algo = std::unique_ptr<NetworKit::DegreeIntervalSwitching> (new NetworKit::DegreeIntervalSwitching(G, degreeIntervals, 1.0));
            dis_algo->setSwitchingTypeDistribution(id_prob, hf_prob, es_prob);
            break;
        case 2:
            dis_algo = std::unique_ptr<NetworKit::DegreeIntervalSwitching> (new NetworKit::DegreeIntervalSwitching(G, degreeIntervals, 1.0));
            dis_algo->setSwitchingTypeDistribution(id_prob, hf_prob, es_prob);
            dis_algo->setSamplingStrategy(NetworKit::DegreeIntervalSampling::DegreeIntervalSampleSingleTuples);
            break;
        case 3:
            dis_algo = std::unique_ptr<NetworKit::DegreeIntervalSwitching> (new NetworKit::DegreeIntervalSwitchingSeparated(G, degreeIntervals, 1.0));
            dis_algo->setSwitchingTypeDistribution(id_prob, hf_prob, es_prob);
        case 4:
            dis_algo = std::unique_ptr<NetworKit::DegreeIntervalSwitching> (new NetworKit::DegreeIntervalSwitchingSeparated(G, degreeIntervals, 1.0));
            dis_algo->setSwitchingTypeDistribution(id_prob, hf_prob, es_prob);
            dis_algo->setSamplingStrategy(NetworKit::DegreeIntervalSampling::DegreeIntervalSampleSingleTuples);
            break;
        }
        NetworKit::AutocorrelationAnalysis analysis(*dis_algo,
                                                    thinnings,
                                                    min_snapshots,
                                                    algo_label,
                                                    graph_label,
                                                    output_fn_prefix + "-" + algo_label + "-" + graph_label + "-" + std::to_string(i) + ".out",
                                                    1,
                                                    max_snapshots);
        analysis.run();
    }
}