#include "ALEdge.h"
#include <cmath>
#include <algorithm>
#include <unordered_set>
#include <iostream>


std::vector<EdgeGroup> MakeEdges(const Mesh &graph) {
    std::vector<EdgeGroup> result(2, {EdgeType::ET_Line, std::monostate(), {}}); // 0 - horizontal, 1 - vertical
    result[0].name = "mesh_horizontal";
    result[1].name = "mesh_vertical";

    auto adj_list = graph.GetEdges();

    for (uint16_t v = 0; v < graph.GetNumVertices(); v++) {
        for (auto x: adj_list[v]) {
            if (v - x == 1) {
                result[0].edges.push_back({x, v});
            }
            if (v - x == graph.GetNumCols()) {
                result[1].edges.push_back({x, v});
            }
        }
    }

    return result;
}

std::vector<EdgeGroup> MakeEdges(const TMesh &graph) {
    std::vector<EdgeGroup> result = MakeEdges((const Mesh &) graph);
    result.push_back({EdgeType::ET_Arc, ArcData{0.05}, {}, "corner_edges"});

    auto adj_list = graph.GetEdges();

    std::unordered_set<uint16_t> corners{
            0,
            (uint16_t) (graph.GetNumCols() - 1),
            (uint16_t) (graph.GetNumCols() * (graph.GetNumRows() - 1)),
            (uint16_t) (graph.GetNumCols() * graph.GetNumRows() - 1),
    };

    for (uint16_t v = 0; v < graph.GetNumVertices(); v++) {
        for (auto x: adj_list[v]) {
            if (corners.find(v) != corners.end() && corners.find(x) != corners.end() && x > v) {
                result.back().edges.push_back({x, v});
            }
        }
    }

    return result;
}

std::vector<EdgeGroup> MakeEdges(const Torus &graph) {
    std::vector<EdgeGroup> result(2, {EdgeType::ET_Line, std::monostate(), {}}); // 0 - horizontal, 1 - vertical
    result[0].name = "mesh_horizontal";
    result[1].name = "mesh_vertical";
    result.push_back({EdgeType::ET_Arc, ArcData{0.05}, {}, "long_horizontal"}); // horizontal
    result.push_back({EdgeType::ET_Arc, ArcData{0.05}, {}, "long_vertical"}); // vertical

    auto adj_list = graph.GetEdges();

    for (uint16_t v = 0; v < graph.GetNumVertices(); v++) {
        for (auto x: adj_list[v]) {
            if (v - x == 1) {
                result[0].edges.push_back({x, v});
            }
            if (v - x == graph.GetNumCols()) {
                result[1].edges.push_back({x, v});
            }
            if (v - x == graph.GetNumCols() - 1) {
                result[2].edges.push_back({x, v});
            }
            if (v - x == (graph.GetNumRows() - 1) * graph.GetNumCols()) {
                result[3].edges.push_back({x, v});
            }
        }
    }

    return result;
}

std::vector<EdgeGroup> MakeEdges(const Ring &graph) {
    std::vector<EdgeGroup> result(1, {EdgeType::ET_Line, std::monostate(), {}, "ring_circular"});

    auto adj_list = graph.GetEdges();
    auto v_count = graph.GetNumVertices();

    for (uint16_t v = 0; v < v_count; v++) {
        for (auto x: adj_list[v]) {
            if ((v_count + (v - x)) % v_count == 1) {
                result[0].edges.push_back({x, v});
            }
        }
    }

    return result;
}

std::vector<EdgeGroup> MakeEdges(const Spidergon &graph) {
    std::vector<EdgeGroup> result(1, {EdgeType::ET_Line, std::monostate(), {}, "spidergon_circular"});

    result.push_back({EdgeType::ET_Line, std::monostate(), {}, "spidergon_diagonal"});

    auto adj_list = graph.GetEdges();
    auto v_count = graph.GetNumVertices();

    for (uint16_t v = 0; v < v_count; v++) {
        for (auto x: adj_list[v]) {
            if ((v_count + (v - x)) % v_count == 1) {
                result[0].edges.push_back({x, v});
            }
            if (v - x == v_count / 2) {
                result[1].edges.push_back({x, v});
            }
        }
    }

    return result;
}

std::vector<EdgeGroup> MakeEdges(const Circulant &graph) {
    std::vector<EdgeGroup> result(graph.GetGenx().size(), {EdgeType::ET_Line, std::monostate(), {}});
    for (int i = 0; i < graph.GetGenx().size(); i++)
        result[i].name = "circulant_generator_" + std::to_string(i);

    auto adj_list = graph.GetEdges();
    auto v_count = graph.GetNumVertices();

    auto genx = graph.GetGenx();

    for (uint16_t v = 0; v < v_count; v++) {
        for (auto x: adj_list[v]) {
            auto it = std::find(genx.begin(), genx.end(), (v_count + v - x) % v_count);
            if (it == genx.end() || ((v - x) == (v_count + 1) / 2 && v > v_count / 2)) {
                continue;
            }
            result[it - genx.begin()].edges.push_back({x, v});
        }
    }

    return result;
}

std::vector<EdgeGroup> MakeEdges(const HyperCirculant &graph) {
    auto adj_list = graph.GetEdges();
    auto v_count = graph.GetNumVertices();

    auto genx = graph.GetGenx();
    std::vector<EdgeGroup> result;
    for (int i = 0; i < genx.size(); i++)
        for (int j = 0; j < genx[i].size(); j++)
            result.push_back({EdgeType::ET_Line, std::monostate(), {},
                              "hypercirculant_generator_" + std::to_string(i) + "_" + std::to_string(j)});

    for (uint16_t v = 0; v < v_count; v++) {
        for (auto x: adj_list[v]) {
            if (std::find(adj_list[x].begin(), adj_list[x].end(), v) != adj_list[x].end()) {
                auto outer = std::min(x % genx.size(), v % genx.size());
                auto to_find = std::min((v_count + x - v) % v_count, v_count - (v_count + x - v) % v_count);
                auto inner = std::find(genx[outer].begin(), genx[outer].end(), to_find)
                             - genx[outer].begin();

                result[outer * genx[0].size() + inner].edges.push_back({x, v});
            } else {
                auto outer = v % genx.size();
                auto to_find = std::min((v_count + x - v) % v_count, v_count - (v_count + x - v) % v_count);
                auto inner = std::find(genx[outer].begin(), genx[outer].end(), to_find)
                             - genx[outer].begin();

                result[outer * genx[0].size() + inner].edges.push_back({v, x, true});
            }
        }
    }

    return result;
}

std::vector<EdgeGroup> MakeEdges(const DMeshDoubleLinked &graph) {
    std::vector<EdgeGroup> result(2, {EdgeType::ET_Arc, ArcData{0.05}, {}});
    result[0].name = "horizontal";
    result[1].name = "vertical";
    result.push_back({EdgeType::ET_Line, std::monostate(), {}, "diagonal"});

    auto adj_list = graph.GetEdges();

    for (uint16_t v = 0; v < graph.GetNumVertices(); v++) {
        for (auto x: adj_list[v]) {
            if (std::abs(v - x) == 1) {
                result[0].edges.push_back({x, v});
            } else if (std::abs(v - x) == graph.GetNumCols()) {
                result[1].edges.push_back({x, v});
            } else {
                result[2].edges.push_back({x, v});
            }

        }
    }

    return result;
}

std::vector<EdgeGroup> MakeEdges(const DMesh &graph) {
    std::vector<EdgeGroup> result(3, {EdgeType::ET_Line, std::monostate(), {}}); // 0 - horizontal, 1 - vertical
    result[0].name = "horizontal";
    result[1].name = "vertical";
    result[2].name = "diagonal";

    auto adj_list = graph.GetEdges();

    for (uint16_t v = 0; v < graph.GetNumVertices(); v++) {
        for (auto x: adj_list[v]) {
            if (v - x == 1) {
                result[0].edges.push_back({x, v});
            }
            if (v - x == graph.GetNumCols()) {
                result[1].edges.push_back({x, v});
            }
            if (v > x) {
                result[2].edges.push_back({x, v});
            }

        }
    }

    return result;
}

std::vector<EdgeGroup> MakeEdges(const DCM &graph) {
    std::vector<EdgeGroup> result(4, {EdgeType::ET_Line, std::monostate(), {}});
    result[0].name = "horizontal";
    result[1].name = "vertical";
    result[2].name = "diagonal_NE-SW";
    result[3].name = "diagonal_NW-SE";

    auto adj_list = graph.GetEdges();

    for (uint16_t v = 0; v < graph.GetNumVertices(); v++) {
        for (auto x: adj_list[v]) {
            if (v - x == 1) {
                result[0].edges.push_back({x, v});
            }
            if (v - x == graph.GetNumCols()) {
                result[1].edges.push_back({x, v});
            }
            if (v - x == graph.GetNumCols() + 1) {
                result[2].edges.push_back({x, v});
            }
            if (v - x == graph.GetNumCols() - 1) {
                result[3].edges.push_back({x, v});
            }
        }
    }

    return result;
}

std::vector<EdgeGroup> MakeEdges(const MCC &graph) {
    std::vector<EdgeGroup> result(3, {EdgeType::ET_Line, std::monostate(), {}});
    result[0].name = "horizontal";
    result[1].name = "vertical";
    result[2].name = "diagonal";

    auto adj_list = graph.GetEdges();

    for (uint16_t v = 0; v < graph.GetNumVertices(); v++) {
        for (auto x: adj_list[v]) {
            if (v - x == 1) {
                result[0].edges.push_back({x, v});
            } else if (v - x == graph.GetNumCols()) {
                result[1].edges.push_back({x, v});
            } else if (v < x) {
                result[2].edges.push_back({x, v});
            }
        }
    }

    return result;
}

std::vector<EdgeGroup> MakeEdges(const XNetwork &graph) {
    std::vector<EdgeGroup> result(3, {EdgeType::ET_Line, std::monostate(), {}});
    result[0].name = "horizontal";
    result[1].name = "vertical";
    result[2].name = "diagonal";

    auto adj_list = graph.GetEdges();

    for (uint16_t v = 0; v < graph.GetNumVertices(); v++) {
        for (auto x: adj_list[v]) {
            if (v < x) {
                if (v - x == 1)
                    result[0].edges.push_back({x, v});
                else if (v - x == graph.GetNumCols())
                    result[1].edges.push_back({x, v});
                else
                    result[2].edges.push_back({x, v});
            }
        }
    }

    return result;
}

std::vector<EdgeGroup> MakeEdges(const CBPMesh &graph) {
    auto result = MakeEdges((const Mesh &) graph);
    result.push_back({EdgeType::ET_Arc, ArcData{0.2}, {}, "right_top"});
    result.push_back({EdgeType::ET_Arc, ArcData{0.2}, {}, "left_top"});

    auto cols = graph.GetNumCols();
    auto rows = graph.GetNumRows();

    auto adj_list = graph.GetEdges();

    for (uint16_t v = 0; v < graph.GetNumVertices(); ++v) {
        for (auto x: adj_list[v]) {
            if (x - v == cols * 2 - 2) {
                result[2].edges.push_back({x, v});
            }
            if (x - v == cols * 2 + 2) {
                result[3].edges.push_back({x, v});
            }
        }
    }

    return result;
}

std::vector<EdgeGroup> MakeEdges(const StarMesh &graph) {
    std::vector<EdgeGroup> result(2, {EdgeType::ET_Line, std::monostate(), {}}); // 0 - horizontal, 1 - vertical
    result[0].name = "horizontal";
    result[1].name = "vertical";
    result.push_back({EdgeType::ET_Arc, ArcData{0.05}, {}, "long_horizontal"});
    result.push_back({EdgeType::ET_Arc, ArcData{0.05}, {}, "long_vertical"});
    result.push_back({EdgeType::ET_Line, std::monostate(), {}, "diagonal"});

    auto cols = graph.GetNumCols();
    auto rows = graph.GetNumRows();

    auto adj_list = graph.GetEdges();

    for (uint16_t v = 0; v < graph.GetNumVertices(); ++v) {
        for (auto x: adj_list[v]) {
            if (v - x == 1) {
                result[0].edges.push_back({x, v});
            } else if (v - x == cols) {
                result[1].edges.push_back({x, v});
            } else if (v - x == 3) {
                result[2].edges.push_back({x, v});
            } else if (v - x == cols * 3) {
                result[3].edges.push_back({x, v});
            } else if (x < v) {
                result[4].edges.push_back({x, v});
            }
        }
    }

    return result;
}

std::vector<EdgeGroup> MakeEdges(const L2Star &graph) {
    std::vector<EdgeGroup> result(2, {EdgeType::ET_Line, std::monostate(), {}}); // 0 - horizontal, 1 - vertical
    result[0].name = "horizontal";
    result[1].name = "vertical";
    result.push_back({EdgeType::ET_Arc, ArcData{0.1}, {}, "long_horizontal"});
    result.push_back({EdgeType::ET_Arc, ArcData{0.1}, {}, "long_vertical"});

    auto cols = graph.GetNumCols();
    auto rows = graph.GetNumRows();

    auto adj_list = graph.GetEdges();

    for (uint16_t v = 0; v < graph.GetNumVertices(); ++v) {
        for (auto x: adj_list[v]) {
            if (v - x == 1) {
                result[0].edges.push_back({x, v});
            } else if (v - x == cols) {
                result[1].edges.push_back({x, v});
            } else if (v - x == 3) {
                result[2].edges.push_back({x, v});
            } else if (v > x) {
                result[3].edges.push_back({x, v});
            }
        }
    }

    return result;
}

std::vector<EdgeGroup> MakeEdges(const ZMesh &graph) {
    return MakeEdges((const DMesh &) graph);
}

std::vector<EdgeGroup> MakeEdges(const FatTree &graph) {
    std::vector<EdgeGroup> result(1, {EdgeType::ET_Line, std::monostate(), {}, "ft_initial"});

    auto adj_list = graph.GetEdges();

    for (uint16_t v = 0; v < graph.GetNumVertices(); ++v) {
        for (auto x: adj_list[v]) {
            if (x > v)
                result[0].edges.push_back({x, v});
        }
    }

    return result;
}
