#include "ALVertex.h"
#include <cmath>
#include <iostream>

VertexLayout MakeGrid(const Mesh &graph, PictureParameters pic) {
    VertexLayout result;
    result.reserve(graph.GetNumVertices());

    Coordinate start = {pic.x_padding, pic.y_padding};
    Coordinate end = {(uint16_t) (pic.x_limit - pic.x_padding),
                      (uint16_t) (pic.y_limit - pic.y_padding)};

    uint16_t step_x = (end.x - start.x) / (graph.GetNumCols() - 1);
    uint16_t step_y = (end.y - start.y) / (graph.GetNumRows() - 1);

    auto node_types = graph.GetNodeTypes();

    uint16_t y = start.y;
    for (int i = 0; i < graph.GetNumRows(); i++) {
        uint16_t x = start.x;
        for (int j = 0; j < graph.GetNumCols(); j++) {
            result.push_back({x, y, node_types[i * graph.GetNumCols() + j]});
            x += step_x;
        }
        y += step_y;
    }

    return result;
}

VertexLayout MakeCircle(const Graph &graph, PictureParameters pic) {
    VertexLayout result;
    result.reserve(graph.GetNumVertices());

    Coordinate center = {(uint16_t) (pic.x_limit / 2), (uint16_t) (pic.y_limit / 2)};
    uint16_t radius = std::min(center.x - pic.x_padding, center.y - pic.y_padding);
    double angle_step = 2 * M_PI / graph.GetNumVertices();

    auto node_types = graph.GetNodeTypes();

    for (uint16_t v = 0; v < graph.GetNumVertices(); v++) {
        result.push_back({(uint16_t) (center.x - (uint16_t)(radius * cos(v * angle_step + M_PI_2))),
                          (uint16_t) (center.y - (uint16_t)(radius * sin(v * angle_step + M_PI_2))),
                          node_types[v]});
    }

    return result;
}

VertexLayout MakeMCCSubgrid(const MCC &graph, const VertexLayout &main_grid) {
    VertexLayout result;
    auto rows = graph.GetNumRows();
    auto cols = graph.GetNumCols();
    result.reserve(graph.GetNumVertices() - cols * rows);

    for (uint16_t i = 0; i < rows - 1; ++i) {
        for (uint16_t j = 0; j < cols - 1; ++j) {
            if (i % 2 == j % 2) {
                result.push_back({(uint16_t)((main_grid[i * cols + j].x + main_grid[(i + 1) * cols + j + 1].x) / 2),
                                  (uint16_t)((main_grid[i * cols + j].y + main_grid[(i + 1) * cols + j + 1].y) / 2),
                                  NT_Crossbar});
            }
        }
    }

    return result;
}

VertexLayout MakeXNSubgrid(const XNetwork &graph, const VertexLayout &main_grid) {
    VertexLayout result;
    auto rows = graph.GetNumRows();
    auto cols = graph.GetNumCols();
    result.reserve(graph.GetNumVertices() - cols * rows);

    for (uint16_t i = 0; i < rows - 1; ++i) {
        for (uint16_t j = 0; j < cols - 1; ++j) {
            result.push_back({(uint16_t)((main_grid[i * cols + j].x + main_grid[(i + 1) * cols + j + 1].x) / 2),
                              (uint16_t)((main_grid[i * cols + j].y + main_grid[(i + 1) * cols + j + 1].y) / 2),
                              NT_Router});

        }
    }

    return result;
}

VertexLayout MakeTree(const TreeBase &graph, PictureParameters pic) {
    std::vector<Coordinate> coordinates(graph.GetNumVertices());

    uint16_t usable_height = pic.y_limit - 2 * pic.y_padding;
    double y_step = graph.GetNumLevels() > 1 ? double(usable_height) / (graph.GetNumLevels() - 1) : 0;

    int num = 0;
    double add_padding = 0;
    auto levels = graph.GetLevels();
    for (int lvl = 0; lvl < graph.GetNumLevels(); ++lvl) {
        const auto& level = levels[lvl];

        uint16_t usable_width = pic.x_limit - 2 * pic.x_padding;
        double x_step = level.size() > 1 ? ((double)usable_width - 2 * add_padding) / (level.size() - 1) : 0;
        uint16_t y = pic.y_limit - (pic.y_padding + lvl * y_step);

        auto node_types = graph.GetNodeTypes();

        for (int i = 0; i < level.size(); ++i) {
            uint16_t x = level.size() > 1
                      ? (uint16_t)(pic.x_padding + add_padding + i * x_step)
                      : pic.x_limit / 2;

            coordinates[level[i]] = {x, y, node_types[num++]};
        }

        if (lvl != levels.size())
            add_padding += x_step * ((double)levels[lvl].size() / levels[lvl + 1].size() - 1) / 2;
    }

    return coordinates;
}

void Rescale(VertexLayout &layout, PictureParameters old_pic, PictureParameters new_pic) {
    double old_w = old_pic.x_limit - 2 * old_pic.x_padding;
    double old_h = old_pic.y_limit - 2 * old_pic.y_padding;
    double new_w = new_pic.x_limit - 2 * new_pic.x_padding;
    double new_h = new_pic.y_limit - 2 * new_pic.y_padding;

    for (auto& coord : layout) {
        double norm_x = (coord.x - old_pic.x_padding) / old_w;
        double norm_y = (coord.y - old_pic.y_padding) / old_h;

        coord.x = uint16_t(norm_x * new_w + new_pic.x_padding);
        coord.y = uint16_t(norm_y * new_h + new_pic.y_padding);
    }
}
