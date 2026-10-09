#include "AutoDetection.h"
#include "Algorithms.h"

#include <unordered_set>
#include <queue>
#include <algorithm>
#include <limits>
#include <iostream>
#include <set>


static std::vector<uint16_t> findShortestCycleFromNode(const std::vector<std::vector<uint16_t>>& graph, uint16_t start) {
    int n = static_cast<int>(graph.size());
    std::vector<int> dist(n, -1);
    std::vector<int> parent(n, -1);
    std::queue<int> q;

    dist[start] = 0;
    q.push(start);

    int min_cycle_len = std::numeric_limits<int>::max();
    std::pair<int, int> cycle_ends = {-1, -1};

    while (!q.empty()) {
        int current = q.front();
        q.pop();

        for (int neighbor : graph[current]) {
            if (dist[neighbor] == -1) {
                dist[neighbor] = dist[current] + 1;
                parent[neighbor] = current;
                q.push(neighbor);
            } else if (neighbor != parent[current]) {
                int cycle_len = dist[current] + dist[neighbor] + 1;
                if (cycle_len < min_cycle_len) {
                    min_cycle_len = cycle_len;
                    cycle_ends = {current, neighbor};
                }
            }
        }
    }

    if (cycle_ends.first == -1)
        return {};
    int u = cycle_ends.first;
    int v = cycle_ends.second;

    std::vector<int> path_u, path_v;
    while (u != -1) {
        path_u.push_back(u);
        u = parent[u];
    }
    while (v != -1) {
        path_v.push_back(v);
        v = parent[v];
    }

    std::reverse(path_u.begin(), path_u.end());
    std::reverse(path_v.begin(), path_v.end());

    int lca_index = 0;
    while (lca_index < path_u.size() && lca_index < path_v.size() &&
           path_u[lca_index] == path_v[lca_index]) {
        lca_index++;
    }
    lca_index--;

    std::vector<uint16_t> cycle;
    for (int i = path_u.size() - 1; i > lca_index; --i)
        cycle.push_back(static_cast<uint16_t>(path_u[i]));
    for (int i = lca_index; i < path_v.size(); ++i)
        cycle.push_back(static_cast<uint16_t>(path_v[i]));

    return cycle;
}


static uint16_t int_pow(uint16_t base, uint16_t exp) {
    uint16_t result = 1;
    while (exp > 0) {
        if (exp % 2 == 1)
            result *= base;
        base *= base;
        exp /= 2;
    }
    return result;
}


static int int_log2(uint32_t x){
    if (x == 0 || (x & (x - 1)) != 0)
        return -1;

    int log = 0;
    while (x >>= 1)
        ++log;
    return log;
};


DetectedTopology isMesh(const std::vector<std::vector<uint16_t>> &adj_list) {
    uint16_t num_nodes = adj_list.size();
    auto count_neig = [&adj_list](uint16_t node) -> uint16_t { return adj_list[node].size(); };

    int root = -1;
    for (uint16_t i = 0; i < num_nodes; i++) {
        if (count_neig(i) == 2) {
            root = i;
            break;
        }
    }

    std::vector<uint16_t> curr_layer;
    curr_layer.reserve(adj_list.size() / 2);
    std::vector<uint16_t> next_layer;
    next_layer.reserve(adj_list.size() / 2);
    std::unordered_set<uint16_t> visited;
    uint16_t rows = 1;
    uint16_t cols = 2;

    /* decide how we will traverse the graph by layers */
    curr_layer.push_back(root);
    visited.insert(root);
    uint16_t f = adj_list[root][0];
    uint16_t s = adj_list[root][1];
    if ((count_neig(f) == 3 || count_neig(f) == 2) && count_neig(s) == 3) {
        curr_layer.push_back(f);
        next_layer.push_back(s);
    } else if ((count_neig(s) == 3 || count_neig(s) == 2) && count_neig(f) == 3) {
        curr_layer.push_back(s);
        next_layer.push_back(f);
    } else {
        return DetectedTopology{"Other", {}};
    }

    /* initializing current and next layers */
    uint16_t node = curr_layer.back();
    while (count_neig(node) == 3) {
        if (count_neig(node) > 4 || count_neig(node) < 2) {
            return DetectedTopology{"Other", {}};
        }
        visited.insert(node);
        uint16_t next_node = node;
        for (const uint16_t neig: adj_list[node]) {
            if ((!visited.count(neig) && count_neig(neig) == 2) ||
                (!visited.count(neig) && count_neig(neig) == 3)) {
                next_node = neig;
                curr_layer.push_back(next_node);
            } else if (!visited.count(neig) && count_neig(neig) == 4) {
                next_layer.push_back(neig);
            }
        }
        if (node == next_node) {
            return DetectedTopology{"Other", {}};
        }
        node = next_node;
        cols += 1;
    }
    if (count_neig(node) != 2) {
        return DetectedTopology{"Other", {}};
    }
    for (const uint16_t neig: adj_list[node]) {
        if (!visited.count(neig)) {
            next_layer.push_back(neig);
        }
    }
    visited.insert(node);

    /* cols is a final number of cols in our mesh topology. Rows is 1, because we've gone through one layer so far */
    if (cols != curr_layer.size() || cols != next_layer.size()) {
        return DetectedTopology{"Other", {}};
    }

    /*
    now we know that the first 2 layers of the graph form the mesh topology.
    now we can go through all the layers and check if they correspond to the mesh topology as well
    */
    curr_layer = std::move(next_layer);
    next_layer.clear();
    while (!curr_layer.empty()) {
        uint16_t expected = 2;
        if (count_neig(curr_layer[0]) == 2) { /* potentially the last layer */
            expected = 1;
        }
        if (curr_layer.size() != cols) {
            return DetectedTopology{"Other", {}};
        }
        for (uint16_t i = 0; i < curr_layer.size(); i++) {
            visited.insert(curr_layer[i]);
            uint16_t unvisited_nodes = 0;
            int next_node = -1;

            if (i == curr_layer.size() - 1) {
                for (const uint16_t neig: adj_list[curr_layer[i]]) {
                    if (!visited.count(neig)) {
                        unvisited_nodes += 1;
                        next_node = neig;
                    }
                }
                if (unvisited_nodes != expected - 1) {
                    return DetectedTopology{"Other", {}};
                }
                if (next_node != -1) {
                    next_layer.push_back(next_node);
                }
            } else {
                uint16_t is_connected = 0;
                for (const uint16_t neig: adj_list[curr_layer[i]]) {
                    if (neig == curr_layer[i + 1] && !visited.count(neig)) {
                        is_connected = 1;
                        unvisited_nodes += 1;
                    } else if (neig == curr_layer[i + 1] && visited.count(neig)) {
                        return DetectedTopology{"Other", {}};
                    } else if (neig != curr_layer[i + 1] && !visited.count(neig)) {
                        unvisited_nodes += 1;
                        next_node = neig;
                    }
                }
                if (!is_connected || unvisited_nodes != expected) {
                    return DetectedTopology{"Other", {}};
                }
                if (next_node != -1) {
                    next_layer.push_back(next_node);
                }
            }
        }
        curr_layer = std::move(next_layer);
        next_layer.clear();
        rows += 1;
    }

    if (rows * cols == num_nodes && visited.size() == num_nodes) {
        return DetectedTopology{"Mesh", {rows, cols}};
    }
    return DetectedTopology{"Other", {}};
}


DetectedTopology isSpidergon(const std::vector<std::vector<uint16_t>>& adj_list) {
    if (adj_list.size() <= 6) {
        return DetectedTopology{"Spidergon", {static_cast<uint16_t>(adj_list.size())}};
    }
    for (uint16_t i = 0; i < adj_list.size(); i++) {
        if (findShortestCycleFromNode(adj_list, i).size() != 4) {
            return DetectedTopology{"Other", {}};
        }
    }
    return DetectedTopology{"Spidergon", {static_cast<uint16_t>(adj_list.size())}};
}


DetectedTopology isRing(const std::vector<std::vector<uint16_t>> &adj_list) {
    std::unordered_set<int> visited;
    std::queue<int> queue;
    queue.push(0);
    while (!queue.empty()) {
        int node = queue.front();
        queue.pop();
        if (adj_list[node].size() != 2) {
            return DetectedTopology{"Other", {}};
        }
        if (visited.find(node) == visited.end()) {
            visited.insert(node);
            for (int neighbor: adj_list[node]) {
                if (visited.find(neighbor) == visited.end()) {
                    queue.push(neighbor);
                }
                if (std::find(adj_list[neighbor].begin(), adj_list[neighbor].end(), node) == adj_list[neighbor].end()) {
                    return DetectedTopology{"Other", {}};
                }
            }
        }
    }
    if (visited.size() == adj_list.size()) {
        return DetectedTopology{"Ring", {static_cast<uint16_t>(adj_list.size())}};
    }
    return DetectedTopology{"Other", {}};
}


DetectedTopology isTorus(const std::vector<std::vector<uint16_t>>& adj_list) {
    auto v = adj_list[0];
    std::sort(v.begin(), v.end());
    uint16_t rows = v[1] + 1;
    if (adj_list.size() % rows != 0) {
        return DetectedTopology{"Other", {}};
    }
    return DetectedTopology{"Torus", {rows, static_cast<uint16_t>(adj_list.size() / rows)}};
}


DetectedTopology isFatTree(const std::vector<std::vector<uint16_t>>& adj_list, std::map<uint16_t, uint16_t>& counter) {
    if (int_log2(adj_list.size() - counter[1] + 1) == -1) {
        return DetectedTopology{"Other", {}};
    }
    uint16_t levels = int_log2(adj_list.size() - counter[1] + 1);
    if (counter[1] % (int_pow(2, (levels - 1))) != 0) {
        return DetectedTopology{"Other", {}};
    }
    return DetectedTopology{"FatTree", {static_cast<uint16_t>(counter[1] / (int_pow(2, (levels - 1)))), levels}};
}


DetectedTopology isXTree(const std::vector<std::vector<uint16_t>>& adj_list) {
    if (int_log2(adj_list.size() + 1) == -1) {
        return DetectedTopology{"Other", {}};
    }
    return DetectedTopology{"XTree", {static_cast<uint16_t>(int_log2(adj_list.size() + 1))}};
}


DetectedTopology isTMesh(const std::vector<std::vector<uint16_t>>& adj_list) {
    auto v = adj_list[0];
    std::sort(v.begin(), v.end());
    uint16_t rows = v[1] + 1;
    if (adj_list.size() % rows != 0) {
        return DetectedTopology{"Other", {}};
    }
    return DetectedTopology{"TMesh", {rows, static_cast<uint16_t>(adj_list.size() / rows)}};
}


DetectedTopology isDMesh(const std::vector<std::vector<uint16_t>>& adj_list) {
    std::vector<uint16_t> nodes3;
    for (uint16_t i = 0; i < adj_list.size(); i++) {
        if (adj_list[i].size() == 3) {
            nodes3.push_back(i);
        }
    }
    uint16_t rows = 65535;
    for (uint16_t i = 0; i < nodes3.size(); i++) {
        for (uint16_t j = 0; j < nodes3.size(); j++) {
            if (i != j) {
                auto path = find_shortest_path(adj_list, nodes3[i], nodes3[j]).size();
                if (path < rows) {
                    rows = path;
                }
            }
        }
    }
    if (adj_list.size() % rows != 0) {
        return DetectedTopology{"Other", {}};
    }
    return DetectedTopology{"DMesh", {rows, static_cast<uint16_t>(adj_list.size() / rows)}};
}


DetectedTopology isDMeshDoubleLinked(const std::vector<std::vector<uint16_t>>& adj_list) {
    std::vector<uint16_t> nodes5;
    for (uint16_t i = 0; i < adj_list.size(); i++) {
        if (adj_list[i].size() == 5) {
            nodes5.push_back(i);
        }
    }
    uint16_t rows = 65535;
    for (uint16_t i = 0; i < nodes5.size(); i++) {
        for (uint16_t j = 0; j < nodes5.size(); j++) {
            if (i != j) {
                uint16_t path = find_shortest_path(adj_list, nodes5[i], nodes5[j]).size();
                if (path < rows) {
                    rows = path;
                }
            }
        }
    }
    if (adj_list.size() % rows != 0) {
        return DetectedTopology{"Other", {}};
    }
    return DetectedTopology{"DMeshDoubleLinked", {rows, static_cast<uint16_t>(adj_list.size() / rows)}};
}


DetectedTopology isZMesh(const std::vector<std::vector<uint16_t>>& adj_list) {
    uint16_t curr = 0;
    for (uint16_t i = 0; i < adj_list.size(); i++) {
        if (adj_list[i].size() == 2) {
            curr = i;
            break;
        }
    }
    uint16_t rows = 1;
    std::set<uint16_t> visited = {curr};
    auto prev = curr;
    while (adj_list[curr].size() != 3) {
        prev = curr;
        for (uint16_t i = 0; i < adj_list[curr].size(); i++) {
            if (adj_list[adj_list[curr][i]].size() == 4 && !(visited.count(adj_list[curr][i]))) {
                rows += 1;
                visited.insert(adj_list[curr][i]);
                curr = adj_list[curr][i];
                break;
            }
            if (adj_list[adj_list[curr][i]].size() == 3 && !(visited.count(adj_list[curr][i]))) {
                curr = adj_list[curr][i];
                rows += 1;
                break;
            }
        }
        if (prev == curr) {
            return DetectedTopology{"Other", {}};
        }
    }
    if (adj_list.size() % rows != 0) {
        return DetectedTopology{"Other", {}};
    }
    return DetectedTopology{"ZMesh", {rows, static_cast<uint16_t>(adj_list.size() / rows)}};
}


DetectedTopology isDCM(const std::vector<std::vector<uint16_t>>& adj_list) {
    std::vector<uint16_t> nodes3;
    for (uint16_t i = 0; i < adj_list.size(); i++) {
        if (adj_list[i].size() == 3) {
            nodes3.push_back(i);
        }
    }
    uint16_t rows = 65535;
    for (uint16_t i = 0; i < nodes3.size(); i++) {
        for (uint16_t j = 0; j < nodes3.size(); j++) {
            if (i != j) {
                uint16_t path = find_shortest_path(adj_list, nodes3[i], nodes3[j]).size();
                if (path < rows) {
                    rows = path;
                }
            }
        }
    }
    if (adj_list.size() % rows != 0) {
        return DetectedTopology{"Other", {}};
    }
    return DetectedTopology{"DCM", {rows, static_cast<uint16_t>(adj_list.size() / rows)}};
}


DetectedTopology isMCC(const std::vector<std::vector<uint16_t>>& adj_list) {
    uint16_t first4 = 0;
    for (auto node : adj_list[0]) {
        if (adj_list[node].size() == 4) {
            first4 = node;
        }
    }
    auto v = adj_list[first4];
    std::sort(v.begin(), v.end());
    uint16_t rows = v[2];
    if (first4 % rows != 0) {
        return DetectedTopology{"Other", {}};
    }
    return DetectedTopology{"MCC", {rows, static_cast<uint16_t>(first4 / rows)}};
}


DetectedTopology isXNetwork(const std::vector<std::vector<uint16_t>>& adj_list) {
    std::vector<uint16_t> nodes3;
    for (uint16_t i = 0; i < adj_list.size(); i++) {
        if (adj_list[i].size() == 3) {
            nodes3.push_back(i);
        }
    }
    uint16_t rows = 65535;
    for (uint16_t i = 0; i < nodes3.size(); i++) {
        for (uint16_t j = 0; j < nodes3.size(); j++) {
            if (i != j) {
                uint16_t path = find_shortest_path(adj_list, nodes3[i], nodes3[j]).size();
                if (path < rows) {
                    rows = path;
                }
            }
        }
    }
    uint16_t cols = (adj_list.size() + rows - 1) / (2 * rows - 1);
    return DetectedTopology{"XNetwork", {rows, cols}};
}


DetectedTopology isCirculant(const std::vector<std::vector<uint16_t>>& adj_list) {
    auto node = adj_list[0];
    std::set<uint16_t> unique_neig;
    std::vector<uint16_t> gens = {static_cast<uint16_t>(adj_list.size())};

    for (uint16_t i = 0; i < node.size() / 2; i++) {
        unique_neig.insert(node[i]);
    }
    gens.insert(gens.end(), unique_neig.begin(), unique_neig.end());

    return DetectedTopology{"Circulant", gens};
}


DetectedTopology isStarMesh(const std::vector<std::vector<uint16_t>>& adj_list, std::map<uint16_t, uint16_t>& counter) {
    if (adj_list.size() % 9 != 0) return DetectedTopology{"Other", {}};
    if (counter[8] != 0 && adj_list.size() == 9) return DetectedTopology{"StarMesh", {3, 3}};
    if (counter[9] != 0) return DetectedTopology{"StarMesh", {3, static_cast<uint16_t>(adj_list.size() / 3)}};
    if (counter[10] != 0) {
        std::vector<uint16_t> nodes10;
        for (uint16_t i = 0; i < adj_list.size(); i++) {
            if (adj_list[i].size() == 10) {
                nodes10.push_back(i);
            }
        }
        uint16_t rows = 65535;
        for (uint16_t i = 0; i < nodes10.size(); i++) {
            for (uint16_t j = 0; j < nodes10.size(); j++) {
                if (i != j) {
                    uint16_t path = find_shortest_path(adj_list, nodes10[i], nodes10[j]).size();
                    if (path < rows) {
                        rows = path;
                    }
                }
            }
        }
        return DetectedTopology{"StarMesh", {static_cast<uint16_t>(rows * 3), static_cast<uint16_t>(3 * (counter[10] / rows))}};
    }
    return DetectedTopology{"Other", {}};
}


DetectedTopology detectTopology(const std::vector<std::vector<uint16_t>>& adj_list) {
    uint16_t n = adj_list.size();

    if (n <= 1) return DetectedTopology{"Other", {}};
    if (n < 4) return DetectedTopology("Ring", {n});

    std::map<uint16_t, uint16_t> counter;
    for (const auto& node : adj_list) {
        counter[node.size()] += 1;
    }

    //for (const auto& [key, value] : counter) {
    //    std::cout << "key: " << key << ", val: " << value << std::endl;
    //}

    if (n == 4) {
        if (counter[2] == 4) return DetectedTopology{"Ring", {4}};
        if (counter[3] == 4) return DetectedTopology{"Spidergon", {4}};
        if (counter[4] == 4) return DetectedTopology{"Torus", {2, 2}};
        if (counter[5] == 4) return DetectedTopology{"DMeshDoubleLinked", {2, 2}};
        if (counter[2] == 2 && counter[3] == 2) return DetectedTopology{"ZMesh", {2, 2}};
    }

    if (counter.count(1) == 1) {
        auto ft = isFatTree(adj_list, counter);
        if (ft.first != "Other") return ft;
        return isMCC(adj_list);
    }

    if (counter.size() == 1) {
        if (counter[2] == n) {
            return isRing(adj_list);
        }
        if (counter[3] == n) {
            return isSpidergon(adj_list);
        }
        if (counter[4] == n) {
            auto tor = isTorus(adj_list);
            if (tor.first != "Other") return tor;
            return isCirculant(adj_list);
        }
        return isCirculant(adj_list);
    }

    if (counter.size() == 2) {
        if (counter.count(2) == 1 && counter.count(3) == 1) return isMesh(adj_list);
        if (counter.count(4) == 1 && counter.count(3) == 1) {
            if (counter[3] == 4) return isDCM(adj_list);
            if (counter[4] >= 4) return isTMesh(adj_list);
        }
        if (counter.count(3) == 1 && counter.count(5) == 1) return isDMesh(adj_list);
        if (counter.count(5) == 1 && counter.count(8) == 1) return isDMeshDoubleLinked(adj_list);
        if (counter.count(3) == 1 && counter.count(8) == 1) return isStarMesh(adj_list, counter);
        return DetectedTopology{"Other", {}};
    }

    if (counter.size() == 3) {
        if (counter.count(2) == 1 && counter.count(3) == 1 && counter.count(4) == 1) {
            auto mesh = isMesh(adj_list);
            if (mesh.first != "Other") return mesh;
            return DetectedTopology{"ZMesh", {2, n / 2}};
        }
        if (counter.count(3) == 1 && counter.count(5) == 1 && counter.count(8) == 1) return isDMesh(adj_list);
        if (counter.count(5) == 1 && counter.count(8) == 1 && counter.count(12) == 1) return isDMeshDoubleLinked(adj_list);
        if (counter.count(3) == 1 && counter.count(4) == 1 && counter.count(5) == 1) return isXNetwork(adj_list);
        if (counter.count(3) == 1 && counter.count(4) == 1 && counter.count(6) == 1) return isDCM(adj_list);
        return DetectedTopology{"Other", {}};
    }

    if (counter.size() == 4) {
        if (counter.count(2) == 1 && counter.count(3) == 1 && counter.count(4) == 1 && counter.count(5) == 1) return isXTree(adj_list);
        if (counter.count(3) == 1 && counter.count(4) == 1 && counter.count(5) == 1 && counter.count(8) == 1) return isXNetwork(adj_list);
        if (counter.count(3) == 1 && counter.count(4) == 1 && counter.count(9) == 1 && counter.count(10) == 1) return isStarMesh(adj_list, counter);
    }
    if (counter.count(2) == 1 && counter.count(3) == 1 && counter.count(4) == 1 && counter.count(5) == 1 && counter.count(6) == 1) return isZMesh(adj_list);
    if (counter.count(3) == 1 && counter.count(4) == 1 && counter.count(5) == 1 && counter.count(10) == 1 && counter.count(12) == 1) return isStarMesh(adj_list, counter);
    return DetectedTopology{"Other", {}};
}
