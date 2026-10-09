#pragma once

#include <vector>
#include <cstdint>
#include <set>
#include <valarray>
#include "../Topologies/GraphTopologies.h"


std::vector<uint16_t> find_shortest_path(const std::vector<std::vector<uint16_t>>& adj_list, uint16_t start, uint16_t finish);

uint16_t count_cut(const std::vector<std::vector<uint16_t>>& adj_list, const std::set<uint16_t>& subsetA, const std::set<uint16_t>& subsetB);

std::pair<std::set<uint16_t>, std::set<uint16_t>> kernighan_lin_partition(const std::vector<std::vector<uint16_t>>& adj_list, uint16_t max_no_improvement = 2);

std::vector<int> BFSDistances(const std::vector<std::vector<uint16_t>> &g, int u);

int GraphDiameter(const Graph& graph, std::vector<std::vector<int>> &distances);

double GraphAveragePathLength(const Graph& graph, std::vector<std::vector<int>> &distances);

int MaxNodeDegree(const Graph& graph);

int NetworkCost(const Graph& g, std::vector<std::vector<int>> &distances);

double PackingDensity(const Graph& g, std::vector<std::vector<int>> &distances);

double ModifiedNetworkCost(const Graph& graph, std::vector<std::vector<int>> &distances);

double ModifiedPackingDensity(const Graph& graph, std::vector<std::vector<int>> &distances);

struct GraphParameters
{
    int diameter;
    double average_distance;
    int max_node_degree;
    int global_network_cost;
    double global_packing_density;
    double modified_network_cost;
    double modified_packing_density;

    std::vector<std::vector<int>> distances;

    Q_GADGET
    Q_PROPERTY(int diameter MEMBER diameter)
    Q_PROPERTY(double average_distance MEMBER average_distance)
    Q_PROPERTY(int max_node_degree MEMBER max_node_degree)
    Q_PROPERTY(int global_network_cost MEMBER global_network_cost)
    Q_PROPERTY(double global_packing_density MEMBER global_packing_density)
    Q_PROPERTY(double modified_network_cost MEMBER modified_network_cost)
    Q_PROPERTY(double modified_packing_density MEMBER modified_packing_density)
};

Q_DECLARE_METATYPE(GraphParameters);
