#pragma once

#include <vector>
#include <cstdint>
#include <variant>
#include <QColor>
#include "GraphTopologies.h"

/**
 * Alias type for pair coordinates of vertices
 */
struct Coordinate {
    int32_t x;
    int32_t y;
    NodeType type;
    bool is_selected = false;
    bool is_default_color = true;
    QColor color = QColor(Qt::gray);
    int32_t x_origin = 0;
    int32_t y_origin = 0;
};

/**
 * Alias type for vector of vertices coordinates
 */
using VertexLayout = std::vector<Coordinate>;

/**
 * Parameters of picture or screen
 * @param x_limit coordinate limit by x-axis
 * @param y_limit coordinate limit by y-axis
 * @param x_padding minimal padding to the closest vertex from the edge by x-axis
 * @param y_padding minimal padding to the closest vertex from the edge by y-axis
 */
struct PictureParameters {
    uint16_t x_limit;
    uint16_t y_limit;
    uint16_t x_padding;
    uint16_t y_padding;
};

/**
 * Edge type enum
 * @value ET_Line - straight line
 * @value ET_Arc - circle arc
 * @value ET_BLine - broken line
 * @value ET_BCurve - Bezier curve
 */
enum class EdgeType : uint8_t {
    ET_Line = 0,
    ET_Arc = 1,
    ET_BLine = 2,
    ET_BCurve = 3,
};

/**
 * @param ratio_radius Ratio of radius to distance between vertices.
 * Negative - for circle center coordinate to the left from edge direction,
 * positive - for circle center coordinate to the right from edge direction
 */
struct ArcData {
    float ratio;
};

/**
 *
 * @param segments_number number of broken line segments
 * @param ratio segments vector of segments offsets, sum of segments along offsets must be 1, sum of segments across offsets must be 0
 */
struct BLineData {

    /**
     * @param along the offset to edge length ratio along the edge
     * @param across the offset to edge length ratio across the edge
     */
    struct SegmentOffset {
        float along; // ratio parallel to the line
        float across; // ratio orthogonal to the line
    };

    uint8_t segments_number;
    std::vector<SegmentOffset> ratio_segment;
};

/**
 * @param start_p start point of Bezier curve
 * @param control_p1 first control point (connected to the start)
 * @param control_p2 second control point (connected to the end)
 * @param end_p end point of Bezier curve
 */
struct BCurveData {
    double along1;
    double across1;
    double along2;
    double across2;
};

/**
 * All edges are bidirectional, formal direction needed for correct handling of curve edge types
 *
 * @param from first vertex
 * @param to second vertex
 */
struct Edge {
    uint16_t from;
    uint16_t to;
    bool is_directed = false;
    mutable bool is_selected = false;
};

/**
 * Group of edges type
 *
 * @param type EdgeType enum value
 * @param edge_type_data data corresponding to edge type,
 * std::monostate used for ET_Line (and all future types without specific data)
 * @param edges vector of edges maintained by group
 */
struct EdgeGroup {
    EdgeType type;
    std::variant<std::monostate, ArcData, BLineData, BCurveData> edge_type_data;
    std::vector<Edge> edges;
    std::string name;
    bool is_default_color = true;
    QColor color = QColor(Qt::gray);
};