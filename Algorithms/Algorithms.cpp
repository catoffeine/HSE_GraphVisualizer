#include <queue>
#include <limits>
#include <algorithm>
#include <cstdlib>
#include "Algorithms.h"


std::vector<uint16_t> find_shortest_path(const std::vector<std::vector<uint16_t>>& adj_list, uint16_t start, uint16_t finish) {
    if (adj_list.size() < 2 || start >= adj_list.size() || finish >= adj_list.size()) {
        return {};
    }
    const uint16_t INF = 65535;
    std::vector<uint16_t> dist(adj_list.size(), INF);
    std::vector<int16_t> prev(adj_list.size(), -1);
    std::queue<uint16_t> q;

    dist[start] = 0;
    q.push(start);

    while (!q.empty()) {
        uint16_t u = q.front();
        q.pop();

        if (u == finish) break;

        for (uint16_t v : adj_list[u]) {
            if (dist[v] == INF) {
                dist[v] = dist[u] + 1;
                prev[v] = u;
                q.push(v);
            }
        }
    }

    if (dist[finish] == INF) return {};

    std::vector<uint16_t> path;
    for (uint16_t v = finish; v != static_cast<uint16_t>(-1); v = prev[v]) {
        path.push_back(v);
    }
    std::reverse(path.begin(), path.end());
    return path;
}

uint16_t count_cut(const std::vector<std::vector<uint16_t>>& adj_list, const std::set<uint16_t>& subsetA, const std::set<uint16_t>& subsetB) {
    uint16_t cut_size = 0;
    for (auto u : subsetA) {
        for (auto v : adj_list[u]) {
            if (subsetB.find(v) != subsetB.end()) {
                ++cut_size;
            }
        }
    }
    return cut_size;
}

std::pair<std::set<uint16_t>, std::set<uint16_t>> kernighan_lin_partition(const std::vector<std::vector<uint16_t>>& adj_list, uint16_t max_no_improvement) {
    if (adj_list.size() < 2) {
        return {};
    }

    uint16_t n = adj_list.size();
    uint16_t half_size = n / 2;
    std::set<uint16_t> subsetA, subsetB;

    for (uint16_t i = 0; i < half_size; ++i) {
        subsetA.insert(i);
    }
    for (uint16_t i = half_size; i < n; ++i) {
        subsetB.insert(i);
    }

    uint16_t best_cut = count_cut(adj_list, subsetA, subsetB);
    std::set<uint16_t> best_subsetA = subsetA, best_subsetB = subsetB;
    uint16_t no_improvement_count = 0;
    while (no_improvement_count < max_no_improvement) {
        bool improvement = false;

        std::vector<std::pair<uint16_t, uint16_t>> swap_candidates;
        for (auto u : subsetA) {
            for (auto v : adj_list[u]) {
                if (subsetB.find(v) != subsetB.end()) {
                    swap_candidates.push_back({u, v});
                }
            }
        }

        for (auto [u, v] : swap_candidates) {
            subsetA.erase(u);
            subsetB.erase(v);
            subsetA.insert(v);
            subsetB.insert(u);

            uint16_t new_cut = count_cut(adj_list, subsetA, subsetB);
            if (new_cut < best_cut) {
                best_cut = new_cut;
                best_subsetA = subsetA;
                best_subsetB = subsetB;
                improvement = true;
                no_improvement_count = 0;
            } else {
                subsetA.insert(u);
                subsetB.insert(v);
            }
        }

        if (!improvement) {
            no_improvement_count++;
        }
    }

    return {best_subsetA, best_subsetB};
}

std::vector<int> BFSDistances(const std::vector<std::vector<uint16_t>> &g, int u) {
    std::vector<int> dist(g.size(), -1);
    std::queue<int> q;
    q.push(u);
    dist[u] = 0;

    while (!q.empty()) {
        int cur = q.front(); q.pop();
        for (int v : g[cur]) {
            if (dist[v] == -1) {
                dist[v] = dist[cur] + 1;
                q.push(v);
            }
        }
    }
    return dist;
}

int GraphDiameter(const Graph &graph, std::vector<std::vector<int>> &distances) {
    int diam = 0;
    auto num_vertices = graph.GetNumVertices();
    for (int u = 0; u < num_vertices; ++u) {
        const auto &dist = distances[u];
        for (int d : dist)
            if (d > diam) diam = d;
    }
    return diam;
}

double GraphAveragePathLength(const Graph &graph, std::vector<std::vector<int>> &distances) {
    long long total = 0, count = 0;
    auto num_vertices = graph.GetNumVertices();
    for (int u = 0; u < num_vertices; ++u) {
        const auto &dist = distances[u];
        for (int d : dist) {
            if (d > 0) {
                total += d;
                count++;
            }
        }
    }
    return count > 0 ? (double)total / count : 0.0;
}

int MaxNodeDegree(const Graph &graph) {
    int max_deg = 0;
    const auto &edges = graph.GetEdges();
    for (const auto &neighbors : edges)
        max_deg = std::max(max_deg, (int)neighbors.size());
    return max_deg;
}

double PackingDensity(const Graph &g, std::vector<std::vector<int>> &distances) {
    int compute_nodes = std::count(g.GetNodeTypes().begin(), g.GetNodeTypes().end(), NT_Node);
    return compute_nodes / (double) NetworkCost(g, distances);
}

double ModifiedNetworkCost(const Graph &graph, std::vector<std::vector<int>> &distances) {
    long long total_cost = 0;
    const auto &g = graph.GetEdges();
    auto num_vertices = graph.GetNumVertices();
    for (int u = 0; u < num_vertices; ++u) {
        if (graph.GetNodeTypes()[u] != NT_Node) continue;
        const auto &dist = distances[u];
        int ecc = *max_element(dist.begin(), dist.end());
        total_cost += ecc * (int)g[u].size();
    }
    return (double)total_cost / num_vertices;
}

double ModifiedPackingDensity(const Graph &graph, std::vector<std::vector<int>> &distances) {
    int compute_nodes = std::count(graph.GetNodeTypes().begin(), graph.GetNodeTypes().end(), NT_Node);
    double cost = ModifiedNetworkCost(graph, distances);
    return cost > 0 ? (double)compute_nodes / cost : 0.0;
}

int NetworkCost(const Graph &g, std::vector<std::vector<int>> &distances) {
    return GraphDiameter(g, distances) * MaxNodeDegree(g);
}
