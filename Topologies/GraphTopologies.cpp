#include "GraphTopologies.h"
#include "Utils.h"

#include <queue>
#include <unordered_set>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <set>
#include <numeric>
#include <QJsonArray>


Graph::Graph(uint16_t num_nodes) : num_vertices(num_nodes) {
    adjacency_list.resize(num_nodes);
    nodes.resize(num_nodes, NT_Node);
}

void Graph::AddEdge(uint16_t node1, uint16_t node2, bool is_directed) {
    if (node1 < adjacency_list.size() && node2 < adjacency_list.size()) {
        adjacency_list[node1].push_back(node2);
        if (!is_directed)
            adjacency_list[node2].push_back(node1);
    }
}

const std::vector<uint16_t> &Graph::GetNeighbors(uint16_t node) const {
    return adjacency_list.at(node);
}

uint16_t Graph::GetNumVertices() const {
    return num_vertices;
}

bool Graph::IsConnected() const {
    if (num_vertices == 0) {
        return true;
    }
    std::unordered_set<int> visited;
    std::queue<int> queue;
    queue.push(0);
    while (!queue.empty()) {
        int node = queue.front();
        queue.pop();
        if (visited.find(node) == visited.end()) {
            visited.insert(node);
            for (int neighbor: adjacency_list[node]) {
                if (visited.find(neighbor) == visited.end()) {
                    queue.push(neighbor);
                }
            }
        }
    }
    return visited.size() == num_vertices;
}


std::vector<std::vector<uint16_t>> Graph::GetAdjacencyMatrix() const {
    std::vector<std::vector<uint16_t>> matrix(num_vertices, std::vector<uint16_t>(num_vertices, 0));
    for (uint16_t i = 0; i < num_vertices; ++i) {
        for (auto j: adjacency_list[i]) {
            matrix[i][j] += 1;
        }
    }
    return matrix;
}

const std::vector<std::vector<uint16_t>> &Graph::GetEdges() const {
    return adjacency_list;
}

void Graph::AddRouters(uint16_t count) {
    num_vertices += count;
    adjacency_list.resize(num_vertices);
    nodes.resize(num_vertices, NT_Router);
}

void Graph::AddCrossbars(uint16_t count) {
    num_vertices += count;
    adjacency_list.resize(num_vertices);
    nodes.resize(num_vertices, NT_Crossbar);
}

const std::vector<NodeType> &Graph::GetNodeTypes() const {
    return nodes;
}

bool Graph::SetNodeTypes(std::vector<NodeType> &&node_types) {
    if (node_types.size() != num_vertices)
        return false;

    nodes = node_types;

    return true;
}

bool Graph::SetEdges(std::vector<std::vector<uint16_t>> &&edges) {
    if (edges.size() != num_vertices)
        return false;

    adjacency_list = edges;

    return true;
}

QJsonObject Graph::toJsonExtra() const {
    return {};
}

Mesh::Mesh(uint16_t rows, uint16_t cols) : Graph(rows * cols), num_rows(rows), num_cols(cols) {
    for (uint16_t i = 0; i < rows; ++i) {
        for (uint16_t j = 0; j < cols; ++j) {
            uint16_t current_vertex = i * cols + j;
            if (i != 0) {
                AddEdge(current_vertex, current_vertex - cols);
            }
            if (j != 0) {
                AddEdge(current_vertex, current_vertex - 1);
            }
        }
    }
}

uint16_t Mesh::GetNumCols() const {
    return num_cols;
}

uint16_t Mesh::GetNumRows() const {
    return num_rows;
}

QJsonObject Mesh::toJsonExtra() const {
    QJsonObject obj;
    obj["num_rows"] = num_rows;
    obj["num_cols"] = num_cols;
    return obj;
}

Torus::Torus(uint16_t rows, uint16_t cols) : Mesh(rows, cols) {
    for (uint16_t i = 0; i < rows; ++i) {
        AddEdge(i * cols, i * cols + cols - 1);
    }
    for (uint16_t i = 0; i < cols; ++i) {
        AddEdge(i, (rows - 1) * cols + i);
    }
}

Ring::Ring(uint16_t num_nodes) : Graph(num_nodes) {
    for (uint16_t i = 0; i < num_nodes; ++i) {
        AddEdge(i, (i + 1) % num_nodes);
    }
}


Spidergon::Spidergon(uint16_t num_nodes) : Ring(num_nodes) {
    for (uint16_t i = 0; i < num_nodes; ++i) {
        if (i < num_nodes / 2 == 0) {
            AddEdge(i, (i + num_nodes / 2) % num_nodes); // Connect to the opposite node
        }
    }
}

Circulant::Circulant(uint16_t num_nodes, const std::vector<uint16_t> &generators) :
        Graph(num_nodes), k(generators.size()), genx(generators) {
    Init();
}

Circulant::Circulant(uint16_t num_nodes, std::vector<uint16_t> &&generators) :
        Graph(num_nodes), k(generators.size()), genx(generators) {
    Init();
}


void Circulant::Init() {
    for (uint16_t i = 0; i < num_vertices; ++i) {
        for (auto gen: genx) {
            if (gen != (num_vertices + 1) / 2 || i < num_vertices / 2)
                AddEdge(i, (i + gen) % num_vertices);
        }
    }
}

const std::vector<uint16_t> &Circulant::GetGenx() const {
    return genx;
}

QJsonObject Circulant::toJsonExtra() const {
    QJsonObject obj;
    QJsonArray gen;
    for (auto x: genx)
    {
        gen.append(x);
    }
    obj["generators"] = gen;
    return obj;
}

TMesh::TMesh(uint16_t rows, uint16_t cols) : Mesh(rows, cols) {
    AddEdge(0, cols - 1);
    AddEdge(0, (rows - 1) * cols);
    AddEdge((rows - 1) * cols, rows * cols - 1);
    AddEdge(cols - 1, rows * cols - 1);
}

DMeshDoubleLinked::DMeshDoubleLinked(uint16_t rows, uint16_t cols) : Mesh(rows, cols) {
    for (uint16_t i = 0; i < rows; ++i) {
        for (uint16_t j = 0; j < cols; ++j) {
            uint16_t current_vertex = i * cols + j;
            if (i != 0) {
                AddEdge(current_vertex, current_vertex - cols);
            }
            if (j != 0) {
                AddEdge(current_vertex, current_vertex - 1);
            }
            if (i != rows - 1 && j != cols - 1) {
                AddEdge(current_vertex, current_vertex + cols + 1);
            }
            if (i != rows - 1 && j != 0) {
                AddEdge(current_vertex, current_vertex + cols - 1);
            }
        }
    }
}

DMesh::DMesh(uint16_t rows, uint16_t cols) : Mesh(rows, cols) {
    for (uint16_t i = 0; i < rows; ++i) {
        for (uint16_t j = 0; j < cols; ++j) {
            uint16_t current_vertex = i * cols + j;
            if (i != rows - 1 && j != cols - 1) {
                AddEdge(current_vertex, current_vertex + cols + 1);
            }
            if (i != rows - 1 && j != 0) {
                AddEdge(current_vertex, current_vertex + cols - 1);
            }
        }
    }
}

ZMesh::ZMesh(uint16_t rows, uint16_t cols) : Mesh(rows, cols) {
    for (uint16_t i = 0; i < rows; ++i) {
        for (uint16_t j = 0; j < cols; ++j) {
            uint16_t current_vertex = i * cols + j;
            if (i != rows - 1 && j != cols - 1 && i % 2 == 1) {
                AddEdge(current_vertex, current_vertex + cols + 1);
            }
            if (i != rows - 1 && j != 0 && i % 2 == 0) {
                AddEdge(current_vertex, current_vertex + cols - 1);
            }
        }
    }
}

DCM::DCM(uint16_t rows, uint16_t cols) : Mesh(rows, cols) {
    for (uint16_t i = 0; i < num_rows - 1; ++i) {
        for (uint16_t j = 0; j < num_cols; ++j) {
            if (i % 2 == j % 2) {
                if (j != num_cols - 1) {
                    AddEdge(num_cols * i + j, num_cols * (i + 1) + j + 1);
                }
            } else if (j != 0) {
                AddEdge(num_cols * i + j, num_cols * (i + 1) + j - 1);
            }
        }
    }
}

MCC::MCC(uint16_t rows, uint16_t cols) : Mesh(rows, cols) {
    adjacency_list.clear();
    adjacency_list.resize(num_vertices);

    uint16_t crossbar_idx = num_vertices;
    AddCrossbars(num_cols / 2 * num_rows / 2 + (num_cols - 1) / 2 * (num_rows - 1) / 2);

    for (uint16_t i = 0; i < num_rows - 1; ++i) {
        for (uint16_t j = 0; j < num_cols - 1; ++j) {
            if (i % 2 == j % 2) {
                AddEdge(crossbar_idx, i * cols + j);
                AddEdge(crossbar_idx, (i + 1) * cols + j);
                AddEdge(crossbar_idx, i * cols + j + 1);
                AddEdge(crossbar_idx, (i + 1) * cols + j + 1);
                crossbar_idx++;
            }
        }
    }

    for (uint16_t i = 0; i < num_cols - 1; ++i) {
        if (i % 2 != 0) {
            AddEdge(i, i + 1);
        }
        if (i % 2 != num_rows % 2) {
            AddEdge((num_rows - 1) * num_cols + i, (num_rows - 1) * num_cols + i + 1);
        }
    }

    for (uint16_t i = 0; i < num_rows - 1; ++i) {
        if (i % 2 != 0) {
            AddEdge(i * num_cols, (i + 1) * num_cols);
        }
        if (i % 2 != num_cols % 2) {
            AddEdge((i + 1) * num_cols - 1, (i + 2) * num_cols - 1);
        }
    }
}

XNetwork::XNetwork(uint16_t rows, uint16_t cols) : Mesh(rows, cols) {
    uint16_t router_idx = num_vertices;
    AddRouters((num_cols - 1) * (num_rows - 1));

    for (uint16_t i = 0; i < rows; ++i) {
        for (uint16_t j = 0; j < cols; ++j) {
            if (i != rows - 1 && j != cols - 1) { // se
                AddEdge(router_idx + i * (cols - 1) + j, i * cols + j);
            }
            if (i != 0 && j != cols - 1) { // ne
                AddEdge(router_idx + i * (cols - 1) + j - cols + 1, i * cols + j);
            }
            if (i != rows - 1 && j != 0) { // sw
                AddEdge(router_idx + i * (cols - 1) + j - 1, i * cols + j);
            }
            if (i != 0 && j != 0) { // nw
                AddEdge(router_idx + i * (cols - 1) + j - cols, i * cols + j);
            }
        }
    }
}

CBPMesh::CBPMesh(uint16_t rows, uint16_t cols) : Mesh(rows, cols) {
    for (uint16_t i = 0; i < rows - 1; i += 2) {
        for (uint16_t j = 0; j < cols; j += 2) {
            if (j < cols - 2) {
                AddEdge(i * cols + j, (i + 2) * cols + j + 2);
            }
            if (j > 1) {
                AddEdge(i * cols + j, (i + 2) * cols + j - 2);
            }
        }
    }
}

StarMesh::StarMesh(uint16_t rows, uint16_t cols) : Mesh(rows, cols) {
    if (rows % 3 || cols % 3) {
        throw std::runtime_error("");
    }

    for (uint16_t i = 1; i < rows; i += 3) {
        for (uint16_t j = 1; j < cols; j += 3) {
            AddEdge(i * cols + j, (i - 1) * cols + j - 1);
            AddEdge(i * cols + j, (i - 1) * cols + j + 1);
            AddEdge(i * cols + j, (i + 1) * cols + j - 1);
            AddEdge(i * cols + j, (i + 1) * cols + j + 1);

            if (i + 3 < rows) {
                AddEdge(i * cols + j, (i + 3) * cols + j);
            }
            if (j + 3 < cols) {
                AddEdge(i * cols + j, i * cols + j + 3);
            }
        }
    }
}

L2Star::L2Star(uint16_t rows, uint16_t cols) : Mesh(rows, cols) {
    if (rows % 3 || cols % 3) {
        throw std::runtime_error("");
    }

    for (uint16_t i = 1; i < rows; i += 3) {
        for (uint16_t j = 1; j < cols; j += 3) {
            if (i + 3 < rows) {
                AddEdge(i * cols + j, (i + 3) * cols + j);
            }
            if (j + 3 < cols) {
                AddEdge(i * cols + j, i * cols + j + 3);
            }
        }
    }

    for (uint16_t i = 2; i < rows; i += 3) {
        for (uint16_t j = 0; j < cols; j += 3) {
            if (i + 3 < rows) {
                AddEdge(i * cols + j, (i + 3) * cols + j);
            }
            if (j + 3 < cols) {
                AddEdge(i * cols + j, i * cols + j + 3);
            }
        }
    }
}

std::vector<uint16_t> GetPaleyGenx(uint16_t num_vertices, uint16_t degree) {
    uint16_t p, k;
    if (!IsPrimePower(num_vertices, p, k))
        return {};

    switch (degree) {
        case 2:
            if (num_vertices % 4 != 1)
                return {};
            break;
        case 3:
            if (num_vertices % 3 != 1)
                return {};
            break;
        case 4:
            if (num_vertices % 8 != 1)
                return {};
            break;
        default:
            return {};
    }

    std::set<uint64_t> residues;
    for (uint64_t i = 1; i < num_vertices; ++i) {
        uint64_t prod = 1;
        for (int j = 0; j < degree; j++)
            prod *= i;
        uint64_t res = prod % num_vertices;
        residues.insert(res);
    }

    std::vector<uint16_t> generators;
    for (uint64_t s: residues) {
        if (s != 0 && s <= num_vertices / 2 && residues.count(num_vertices - s)) {
            generators.push_back(s);
        }
        if (residues.count(num_vertices - s) == 0) {
            std::cout << "!!!" << s << "\n";
        }
    }

    std::sort(generators.begin(), generators.end());
    return generators;
}

std::vector<uint16_t> GetMultiplicativeCirculantGenx(uint16_t base, uint16_t degree) {
    std::vector<uint16_t> generators(degree);
    uint16_t prod = 1;
    for (uint16_t i = 0; i < degree; i++) {
        generators[i] = prod;
        prod *= base;
    }
    return generators;
}

HyperCirculant::HyperCirculant(uint16_t num_nodes, const std::vector<std::vector<uint16_t>> &generators) :
        Graph(num_nodes), k(generators[0].size()), genx(generators) {
    Init();
}

HyperCirculant::HyperCirculant(uint16_t num_nodes, std::vector<std::vector<uint16_t>> &&generators) :
        Graph(num_nodes), k(generators[0].size()), genx(generators) {
    Init();
}

const std::vector<std::vector<uint16_t>> &HyperCirculant::GetGenx() const {
    return genx;
}

void HyperCirculant::Init() {
    uint16_t degree = genx.size();
    for (uint16_t i = 0; i < degree; ++i) {
        for (uint16_t node = i; node < num_vertices; node += degree) {
            for (int gen: genx[i]) {
                AddEdge(node, (node + gen + num_vertices) % num_vertices, true);
                AddEdge(node, (node - gen + num_vertices) % num_vertices, true);
            }
        }
    }
}

QJsonObject HyperCirculant::toJsonExtra() const {
    QJsonObject obj;
    QJsonArray gen;
    for (const auto& row : genx) {
        QJsonArray rowArray;
        for (uint16_t v : row)
            rowArray.append(int(v));
        gen.append(rowArray);
    }
    obj["hyper_generators"] = gen;
    return obj;
}

TreeBase::TreeBase(uint16_t num_nodes, uint16_t levels) :
        Graph(num_nodes), levels(levels) {
    tree_levels.reserve(levels + 1);
}

uint16_t TreeBase::GetNumLevels() const {
    return levels + 1;
}

const std::vector<std::vector<uint16_t>> &TreeBase::GetLevels() const {
    return tree_levels;
}

QJsonObject TreeBase::toJsonExtra() const {
    QJsonObject obj;
    QJsonArray tree_levels_json;
    for (const auto& row : tree_levels) {
        QJsonArray rowArray;
        for (uint16_t v : row)
            rowArray.append(int(v));
        tree_levels_json.append(rowArray);
    }
    obj["levels"] = levels;
    obj["tree_levels"] = tree_levels_json;
    return obj;
}

void TreeBase::SetTreeLevels(std::vector<std::vector<uint16_t>> &&new_tree_levels) {
    tree_levels = new_tree_levels;
}

FatTree::FatTree(uint16_t concentration, uint16_t levels) :
        TreeBase(0, levels), conc(concentration) {}

void FatTree::Build() {
    int num_leaves = 1 << (levels - 1);
    num_vertices = num_leaves * conc;
    adjacency_list.resize(num_vertices);
    nodes.resize(num_vertices, NT_Node);

    uint16_t next_router_index = num_vertices;
    std::vector<uint16_t> level(num_vertices);
    std::iota(level.begin(), level.end(), 0);
    if (!level.empty())
        tree_levels.push_back(level);

    for (int lvl = 0; lvl < levels; ++lvl) {
        uint16_t count = 1 << (levels - lvl - 1);
        level.resize(count);
        std::iota(level.begin(), level.end(), next_router_index);
        next_router_index += count;
        tree_levels.push_back(level);
    }

    AddRouters(next_router_index - num_vertices);

    if (conc > 0)
    {
        const auto &leaves = tree_levels[1];
        int node_idx = 0;
        for (int leaf: leaves) {
            for (int i = 0; i < conc; ++i) {
                AddEdge(leaf, node_idx++);
            }
        }
    }

    for (size_t lvl = conc > 0 ? 1 : 0; lvl + 1 < tree_levels.size(); ++lvl) {
        const auto &curr = tree_levels[lvl];
        const auto &next = tree_levels[lvl + 1];

        for (size_t i = 0; i < next.size(); ++i) {
            int parent = next[i];
            size_t left = 2 * i;
            size_t right = 2 * i + 1;

            if (left < curr.size()) AddEdge(parent, curr[left]);
            if (right < curr.size()) AddEdge(parent, curr[right]);
        }
    }
}

QJsonObject FatTree::toJsonExtra() const {
    auto obj = TreeBase::toJsonExtra();
    obj["concentration"] = conc;
    return obj;
}

XTree::XTree(uint16_t levels) : FatTree(0, levels) {}

void XTree::Build() {
    FatTree::Build();

    for (auto &nt: nodes) {
        nt = NT_Node;
    }

    for (const auto &level: tree_levels) {
        for (int i = 0; i < level.size() - 1; i++) {
            AddEdge(level[i], level[i + 1]);
        }
    }

    levels--;
}

