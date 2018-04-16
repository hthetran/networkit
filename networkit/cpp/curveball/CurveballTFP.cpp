/*
 * CurveballTFP.cpp
 *
 *  Created on: Jul 12 2017
 *      Author: Hung Tran, Manuel Penschuck
 */

#include <iostream>
#include "CurveballTFP.h"
#include "../graph/GraphBuilder.h"

#include "../auxiliary/IntSort.h"
#include "../auxiliary/Timer.h"

#include "CurveballHelper.h"

namespace CurveballImpl {

	using degree_vector = std::vector<degree_t>;
	using trade_vector = std::vector<TradeDescriptor>;
	using node_vector = std::vector<node_t>;
	using nodepair_vector = std::vector< std::pair<node_t, node_t> >;

	CurveballTFP::CurveballTFP(const NetworKit::Graph& G)
		: CurveballBase(G)
          , _max_degree(0)
		  , _aff_edges(0)
	{
		hasRun = false;
		assert(G.checkConsistency());
		assert(G.numberOfSelfLoops() == 0);
		assert(_num_nodes > 0);
	}

	std::vector<depchain_msg> CurveballTFP::get_tradelist(const trade_vector& trades) const {
		std::vector<depchain_msg> tlist;
		tlist.reserve(2*trades.size() + 1);

		const auto max_nodes = _G.numberOfNodes();

		tradeid_t i=0;
		for(const auto trade : trades) {
			assert(trade.first < max_nodes);
			assert(trade.second < max_nodes);

			tlist.emplace_back(trade.first, i++);
			tlist.emplace_back(trade.second, i++);
		}

		// padding
		tlist.emplace_back(max_nodes, std::numeric_limits<tradeid_t>::max());

		intsort::sort(
			tlist,
			[](const depchain_msg& msg){return msg.node;},
			max_nodes
		);

		return tlist;
	}

	template <typename GetNeigh>
	void CurveballTFP::build_depchain(const trade_vector& trades, GetNeigh get_neighbors, Aux::SignalHandler& handler) {
        //for(auto t : trades) std::cout << "trade [" << t.first << ", " << t.second << "]\n";

		auto tlist = get_tradelist(trades);

		const auto max_nodes = _G.numberOfNodes();

		std::vector< std::pair<tradeid_t, tradeid_t> > depchain_tmp;
		pq_t<node_t, depchain_msg> pq;

		// insert bottom element into pq to avoid empty() checks
		pq.emplace(std::numeric_limits<node_t>::max(), 0, 0);

		depchain_tmp.reserve(2*trades.size());

		constexpr auto no_more_trade = std::numeric_limits<tradeid_t>::max();

        std::vector<node_t> handled_nodes;
        pq_t<node_t, depchain_msg>::bucket_data_type messages;

		auto it = tlist.begin();
		for(node_t u=0; u < max_nodes; u++) {
		    handler.assureRunning();

            // Obtain and sort neighbors
            auto neighbors = get_neighbors(u);
            if (neighbors.empty()) continue;
            std::sort(neighbors.begin(), neighbors.end());

			// Skip nodes without trade
			if (it->node != u) {
				assert(it->node > u);
                assert(!pq.empty());
				assert(pq.peak_top_key() >= u);

                //std::cout << "Skip u=" << u << " pq.peak_top().first=" << pq.peak_top_key() << " size: "  << pq.size() << "\n";

                handled_nodes.clear();
				for(; pq.peak_top_key() == u; pq.pop()) {
                    //std::cout << " Bypass u=" << u << " to " << pq.top().second.next_trade << "\n";
					_cbpq.emplace(pq.top().second.next_trade, u, no_more_trade);
                    handled_nodes.emplace_back(pq.top().second.node);
				}

                std::sort(handled_nodes.begin(), handled_nodes.end());
                handled_nodes.push_back(std::numeric_limits<node_t>::max());

                // we have to copy the edge info of all unhandled edges to the output
                auto h_it = handled_nodes.cbegin();
                for(const node_t v : neighbors) {
                    if (v >= u) break;

                    while(*h_it < v) h_it++;

                    if (*h_it != v) {
                        _edges.emplace_back(u, v);
                        _edges.emplace_back(v, u);
                    }
                }

                continue;
			}

			// Fetch first trade of this node and build dep chain
			tradeid_t first_tid = no_more_trade;
			{
				// Find all info addressed to this node and sort them by trade-ids
                #ifndef NDEBUG
				auto end = std::find_if_not(it, tlist.end(),
				    [u] (const depchain_msg& msg) {return msg.node == u;});
				assert(end != tlist.end()); // there's a bottom element

                auto comp = [] (const depchain_msg& a, const depchain_msg& b) {
                    return a.next_trade < b.next_trade;
                };

                assert(std::is_sorted(it, end, comp));
                #endif

				// Emit dependency chain
				first_tid = it->next_trade;
				tradeid_t prev_tid = first_tid;

				for (++it; it->node == u; ++it) {
					depchain_tmp.emplace_back(prev_tid, it->next_trade);
					prev_tid = it->next_trade;
				}

				depchain_tmp.emplace_back(prev_tid, no_more_trade);
			}

			// Receive TFP messages
            messages.clear();
			if (pq.peak_top_key() == u) {
				pq.swap_top_bucket(messages);

				/*for(auto& msg:messages) {
				    std::cout << msg.node << " ";
				}
				std::cout << "\n"; */
				//assert(std::is_sorted(messages.crbegin(), messages.crend()));

				std::sort(messages.begin(), messages.end());
			}
            messages.emplace_back(std::numeric_limits<node_t>::max(), 0);

			auto m_it = messages.cbegin();

			for(const node_t v : neighbors) {
				if (v < u) {
					// the neighbor was already processed; if there's no message
					// the neighbor is not traded
					assert(m_it->node >= v);

					if (m_it->node == v) {
						if (first_tid < m_it->next_trade) {
							_cbpq.emplace(first_tid, v, m_it->next_trade);
						} else {
							_cbpq.emplace(m_it->next_trade, u, first_tid);
						}
						m_it++;

					} else {
                        _cbpq.emplace(first_tid, v, no_more_trade);

					}

				} else {
					pq.emplace(v, u, first_tid);
                    //std::cout << "PQ send (" << v << ", (" << u << " @ " << first_tid << "))\n";

				}
			}
		}

		// sort depchain and extract successor info
		const tradeid_t entries = 2*trades.size();
		intsort::sort(depchain_tmp, [] (const auto& p) {return p.first;}, entries);
		_trade_successor.clear();
		_trade_successor.reserve(entries);
		for(tradeid_t i=0; i<entries; i++) {
			assert(depchain_tmp[i].first == i);
			_trade_successor.push_back(depchain_tmp[i].second);
		}

        _cbpq.emplace(no_more_trade, 0, 0);
	}


	void CurveballTFP::load_from_graph(const trade_vector& trades, Aux::SignalHandler& handler) {
		Aux::Timer timer;
		timer.start();

		_edges.reserve(2*_G.numberOfEdges()+1);

        build_depchain(trades, [this] (node_t u) {
			auto tmp = _G.neighbors(u);

            if (tmp.size() > _max_degree)
                _max_degree = tmp.size();

            return tmp;
		}, handler);

		timer.stop();
        std::cout << "Loading initial graph took " << timer.elapsedMilliseconds() << " milliseconds.\n";
	}

	void CurveballTFP::restructure_graph(const trade_vector& trades, Aux::SignalHandler& handler) {
		Aux::Timer timer;
		timer.start();

        const auto max_nodes = _G.numberOfNodes();

        nodepair_vector old_edges;
        old_edges.swap(_edges);

		// we need padding to avoid end-of-vector checks
        intsort::sort(old_edges, [] (const edge_t e) {return e.first;}, max_nodes);
        assert(old_edges.back().first == max_nodes);

		auto begin = old_edges.cbegin();
        std::vector<node_t> neighbors;
        auto get_neighbors = [&begin, &neighbors, max_nodes] (const node_t u) {
			assert(u < max_nodes);

			while(begin->first < u) begin++;
			auto end = begin;
			while(end->first == u) end++;

            neighbors.clear();
			neighbors.reserve(std::distance(begin, end));
			for(auto it = begin; it != end; it++)
				neighbors.push_back(it->second);

            begin = end;

			return neighbors;
		};

        _edges.reserve(old_edges.size());
		build_depchain(trades, get_neighbors, handler);

		timer.stop();
        std::cout << "Restructuring took " << timer.elapsedMilliseconds() << " milliseconds.\n";
	}

	void CurveballTFP::run(const trade_vector& trades) {
        Aux::SignalHandler handler;

		if (!hasRun)
			load_from_graph(trades, handler);
		else
			restructure_graph(trades, handler);

		Aux::Timer timer;
		timer.start();

		NetworKit::count trade_count = 0;

		using neighbour_vector = std::vector< depchain_msg >;
		neighbour_vector neigh_u;
		neighbour_vector neigh_v;
		neighbour_vector disjoint_neighbours;

		neigh_u.reserve(_max_degree);
		neigh_v.reserve(_max_degree);
		disjoint_neighbours.reserve(_max_degree);

        tradeid_t tid = 0;

        auto& urng = Aux::Random::getURNG();

        for (const auto& trade : trades) {
			handler.assureRunning();

			// Trade partners u and v
			const node_t u = trade.first;
			const node_t v = trade.second;
			const tradeid_t next_u = _trade_successor[tid];
			const tradeid_t next_v = _trade_successor[tid+1];

			// Fetch adjacency infos from pq
			bool edge_between_uv = false;
			neigh_u.clear();
			neigh_v.clear();
			{
				assert(_cbpq.top().first >= tid);

                if (_cbpq.peak_top_key() == tid) {
                    _cbpq.swap_top_bucket(neigh_u);
                    std::sort(neigh_u.begin(), neigh_u.end());

                    auto it = std::lower_bound(neigh_u.begin(), neigh_u.end(), v,
                        [] (const depchain_msg& x, node_t v) {return x.node < v;});

                    if (it != neigh_u.end() && it->node == v) {
                        edge_between_uv = true;
                        neigh_u.erase(it);
                    }
                }

				if (_cbpq.peak_top_key() == tid + 1) {
					_cbpq.swap_top_bucket(neigh_v);
                    std::sort(neigh_v.begin(), neigh_v.end());
				}
			}

			_aff_edges += neigh_u.size() + neigh_v.size() + edge_between_uv;

            /*
            auto report = [&] (tradeid_t tid, node_t u, tradeid_t next, auto neigh) {
                std::cout << tid << " " << "node: " << u << " next traded " << next << " neigh("
                    << _G.degree(u) << "):";
                for(auto x : neigh) std::cout << " (" << x.node << " @ " << x.next_trade << ")";
                std::cout << "\n";
            };

            report(tid, u, next_u, neigh_u);
            report(tid+1, v, next_v, neigh_v);
            */

            assert(neigh_u.size() + edge_between_uv == _G.degree(u));
            assert(neigh_v.size() + edge_between_uv == _G.degree(v));


            auto send_edge = [this] (node_t x, tradeid_t next_trade,
                                     const depchain_msg& neigh) {

                if (next_trade < neigh.next_trade) {
                    _cbpq.emplace(next_trade, neigh);

                } else if (next_trade > neigh.next_trade) {
                    _cbpq.emplace(neigh.next_trade, x, next_trade);

                } else {
                    assert(next_trade == std::numeric_limits<tradeid_t>::max());
                    _edges.emplace_back(x, neigh.node);
                    _edges.emplace_back(neigh.node, x);

                }
            };

            // Split neighborhoods into common and disjoint neighbors
            size_t num_common = 0;
            {
                disjoint_neighbours.clear();
                auto u_nit = neigh_u.cbegin();
                auto v_nit = neigh_v.cbegin();

                while ((u_nit != neigh_u.cend()) && (v_nit != neigh_v.cend())) {
                    assert(u_nit->node != v);
                    assert(v_nit->node != u);

                    if (u_nit->node > v_nit->node) {
                        disjoint_neighbours.push_back(*v_nit);
                        v_nit++;
                    } else if (u_nit->node < v_nit->node) {
                        disjoint_neighbours.push_back(*u_nit);
                        u_nit++;
                    } else { // u_nit->node == v_nit->node
                        // common edge -- directly forward it
                        depchain_msg common{u_nit->node,
                            std::min(u_nit->next_trade, v_nit->next_trade)};

                        send_edge(u, next_u, common);
                        send_edge(v, next_v, common);

                        u_nit++;
                        v_nit++;
                        num_common++;
                    }
                }

                if (u_nit != neigh_u.cend()) {
                    disjoint_neighbours.insert(disjoint_neighbours.end(),
                                               u_nit, neigh_u.cend());
                } else if (v_nit != neigh_v.cend()) {
                    disjoint_neighbours.insert(disjoint_neighbours.end(),
                                               v_nit, neigh_v.cend());
                }
            }

			// Shuffle and send disjoint edges
			{
				const size_t u_setsize = neigh_u.size() - num_common;
				const size_t v_setsize = neigh_v.size() - num_common;
                assert(u_setsize + v_setsize == disjoint_neighbours.size());

                random_partition(disjoint_neighbours.begin(), disjoint_neighbours.end(),
                                 u_setsize, urng);

                size_t i = 0;
                for (; i < u_setsize; i++) {
                    send_edge(u, next_u, disjoint_neighbours[i]);
                }


                for (; i < setsize; i++) {
                    send_edge(v, next_v, disjoint_neighbours[i]);
                }
			}

			// Do not forget edge between u and v
			if (edge_between_uv)
				send_edge(u, next_u, {v, next_v});

			trade_count++;
            tid += 2;
		}

        assert(_cbpq.size() == 1);
        _cbpq.clear();

        assert(_edges.size() == 2*_G.numberOfEdges());
        _edges.emplace_back(_G.numberOfNodes(), _G.numberOfNodes()); // padding

		hasRun = true;

		timer.stop();
		std::cout << "Trading took " << timer.elapsedMilliseconds() << " milliseconds.\n";
	}


	NetworKit::Graph CurveballTFP::getGraph() const {
		Aux::Timer timer;
		timer.start();

        NetworKit::GraphBuilder builder(_G.numberOfNodes(), false, false);

        auto it = _edges.cbegin();
        auto end = std::next(it, _edges.size()-1);

        for(; it != end; it++) {
            builder.addHalfEdge(it->first, it->second);
        }

		timer.stop();
        std::cout << "Graph building took " << timer.elapsedMilliseconds() << " milliseconds.\n";

        return builder.toGraph(false, true);
	}

	nodepair_vector CurveballTFP::getEdges() const {
		return _edges;
	}
}
