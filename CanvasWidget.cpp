#include "CanvasWidget.h"
#include "globals.h"

#include <ALTypes.h>
#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QDateTime>
#include <cmath>
#include <QApplication>
#include <QMouseEvent>
#include <qqmlapplicationengine.h>

void DrawArrowHead(QPainter &painter, const QPointF &start, const QPointF &end, const QColor &arrow_color,
    qreal arrow_length = 10, qreal scale = 1.0) {
    QPointF dir = end - start;
    arrow_length = arrow_length / scale;
    qreal length = dir.x() * dir.x() + dir.y() * dir.y();
    if (length > 0.0) {
        dir /= std::sqrt(length);
    }

    QPointF perp1(dir.y(), -dir.x());
    QPointF perp2(-dir.y(), dir.x());

    QPointF p1 = end - dir * arrow_length + perp1 * (arrow_length / 3.0);
    QPointF p2 = end - dir * arrow_length + perp2 * (arrow_length / 3.0);
    QPointF p3 = end;

    QPolygonF arrowHead;
    arrowHead << p1 << p2 << p3;

    painter.setPen(QPen(arrow_color, 2.0 / scale));
    painter.setBrush(arrow_color);
    painter.drawPolygon(arrowHead);
}

QPointF CalculateBezierPoint(double t, const QPointF &P0, const QPointF &P1,
                              const QPointF &P2, const QPointF &P3) {
    double u = 1.0 - t;
    double tt = t * t;
    double uu = u * u;
    double uuu = uu * u;
    double ttt = tt * t;

    QPointF point;
    point.setX(uuu * P0.x() + 3 * uu * t * P1.x() + 3 * u * tt * P2.x() + ttt * P3.x());
    point.setY(uuu * P0.y() + 3 * uu * t * P1.y() + 3 * u * tt * P2.y() + ttt * P3.y());

    return point;
}

std::vector<QPointF> DrawBezierCurve(QPainter *painter, const QPointF &P0, const QPointF &P1,
                         const QPointF &P2, const QPointF &P3, int segments=100) {
    QPointF prev_point = P0;

    std::vector<QPointF> curve_points;

    for (int i = 1; i <= segments; ++i) {
        double t = static_cast<double>(i) / segments;
        QPointF current_point = CalculateBezierPoint(t, P0, P1, P2, P3);
        curve_points.push_back(current_point);

        painter->drawLine(prev_point, current_point);

        prev_point = current_point;
    }

    return curve_points;
}

CanvasWidget::CanvasWidget(QQuickPaintedItem *parent)
    : QQuickPaintedItem(parent) {

    setAcceptedMouseButtons(Qt::AllButtons);
    setFlag(ItemAcceptsInputMethod, true);
    setFlag(ItemHasContents, true);
}


TempCanvas::TempCanvas(QWidget *parent) : QWidget(parent) {}

QImage TempCanvas::DrawCanvas(QSvgGenerator *generator) {
    return DrawGraph(generator);
}

QImage TempCanvas::DrawGraph(QSvgGenerator *generator) {
    this->nodes_ptr = PAGE_MANAGER.GetNodes();
    this->edges_ptr = PAGE_MANAGER.GetEdges();

    QPointF minPoint(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());
    QPointF maxPoint(-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity());

    for (const auto &node : *nodes_ptr) {
        minPoint.setX(std::min(minPoint.x(), static_cast<qreal>(node.x)));
        minPoint.setY(std::min(minPoint.y(), static_cast<qreal>(node.y)));
        maxPoint.setX(std::max(maxPoint.x(), static_cast<qreal>(node.x)));
        maxPoint.setY(std::max(maxPoint.y(), static_cast<qreal>(node.y)));
    }

    QSize widgetSize(maxPoint.x() + 100, maxPoint.y() + 100);

    QPainter painter;
    QImage image(widgetSize, QImage::Format_ARGB32);

    if (!generator) {
        image.fill(Qt::white);

        painter.begin(&image);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    } else {
        generator->setSize(widgetSize);
        generator->setViewBox(QRect(0, 0, widgetSize.width(), widgetSize.height()));

        painter.begin(generator);
    }


    QColor node_color = QColor(Qt::black);
    QColor text_color = QColor("#848484");
    QColor background_color = QColor(Qt::white);

    for (const auto &group_edge : *edges_ptr) {
        for (auto &edge : group_edge.edges) {
            auto from_node = &((*nodes_ptr)[edge.from]);
            auto to_node = &((*nodes_ptr)[edge.to]);

            if (from_node && to_node) {
                if (!group_edge.is_default_color) {
                    painter.setPen(QPen(group_edge.color, SETTINGS.edges_width));
                    painter.setBrush(group_edge.color);
                }
                else {
                    painter.setPen(QPen(node_color, SETTINGS.edges_width));
                    painter.setBrush(node_color);
                }


                const QPointF P_start(from_node->x, from_node->y);
                const QPointF P_end(to_node->x, to_node->y);

                if (group_edge.type == EdgeType::ET_BCurve) {
                    auto data = std::get<BCurveData>(group_edge.edge_type_data);

                    QPointF direction = P_end - P_start;
                    double length = std::hypot(direction.x(), direction.y());

                    QPointF dir_unit = direction / length;
                    QPointF perp_unit(-dir_unit.y(), dir_unit.x());
                    QPointF P_ref1 = P_start
                                    + data.along1 * direction
                                    + data.across1 * length * perp_unit;

                    QPointF P_ref2 = P_end
                                    - data.along2 * direction
                                    - data.across2 * length * perp_unit;
                    std::vector<QPointF> points = DrawBezierCurve(&painter, P_start, P_ref1, P_ref2, P_end);
                } else if (group_edge.type == EdgeType::ET_Arc) {
                    qreal x1 = from_node->x;
                    qreal y1 = from_node->y;
                    qreal x2 = to_node->x;
                    qreal y2 = to_node->y;

                    qreal midX = (x1 + x2) / 2.0;
                    qreal midY = (y1 + y2) / 2.0;

                    qreal dx = x2 - x1;
                    qreal dy = y2 - y1;

                    qreal d = 0.5 * std::hypot(dx, dy);

                    qreal length = std::hypot(dx, dy);
                    qreal perpX = -dy / length;
                    qreal perpY = dx / length;

                    qreal centerX = midX + perpX * d;
                    qreal centerY = midY + perpY * d;

                    qreal radius = std::hypot(x1 - centerX, y1 - centerY);

                    qreal startAngleRad = std::atan2(y1 - centerY, x1 - centerX);
                    qreal endAngleRad = std::atan2(y2 - centerY, x2 - centerX);

                    int startAngle = static_cast<int>(-startAngleRad * 180.0 / M_PI * 16);
                    int endAngle = static_cast<int>(-endAngleRad * 180.0 / M_PI * 16);

                    int spanAngle = endAngle - startAngle;

                    if (spanAngle > 0) spanAngle -= 360 * 16;

                    QRectF rect(centerX - radius, centerY - radius, radius * 2, radius * 2);
                    painter.drawArc(rect, startAngle, spanAngle);
                } else {
                    painter.drawLine(P_start, P_end);
                }
                if (edge.is_directed)
                    DrawArrowHead(painter, P_start, P_end, node_color, 10, 1.0);
            }
        }
    }

    uint16_t node_index = 0;
    for (auto &node : *nodes_ptr) {
        painter.setOpacity(1.0);
        painter.setPen(Qt::transparent);
        if (!node.is_default_color) painter.setBrush(node.color);
        else painter.setBrush(node_color);

        if (node.type == 0 || node.type == 1) {
            qreal radius = SETTINGS.nodes_radius;
            QRectF ellipse(node.x - radius, node.y - radius, 2 * radius, 2 * radius);

            if (node.type == 0) {
                painter.setPen(Qt::NoPen);
                painter.drawEllipse(ellipse);
            } else if (node.type == 1) {
                painter.setBrush(background_color);
                painter.setPen(Qt::NoPen);
                painter.drawEllipse(ellipse);

                painter.setBrush(Qt::NoBrush);
                painter.setPen(QPen(node_color, SETTINGS.nodes_radius));
                painter.drawEllipse(ellipse);
            }
        } else if (node.type == 2) {
            qreal diff = SETTINGS.nodes_radius + 4;
            QRectF rect(node.x - diff, node.y - diff, 2 * diff, 2 * diff);
            painter.drawRect(rect);
        }

        painter.setOpacity(1.0);

        if (SETTINGS.show_label_text) {
            painter.setPen(text_color);
            painter.setFont(QFont("monospace", SETTINGS.font_size));
            painter.drawText(node.x, node.y - 18.0, QString::number(node_index));
        }

        node_index++;
    }

    return image;
}


void CanvasWidget::SetSettings() {
    node_color = SETTINGS.dark_mode ? node_color_dark : node_color_white;
    node_connection_color = SETTINGS.dark_mode ? node_connection_color_dark : node_connection_color_white;
    background_color = SETTINGS.dark_mode ? background_color_dark : background_color_white;
    grid_color = SETTINGS.dark_mode ? grid_color_dark : grid_color_white;
    text_color = SETTINGS.dark_mode ? text_color_dark : text_color_white;
    selection_color = SETTINGS.dark_mode ? selection_color_dark : selection_color_white;
    node_hovered_color = SETTINGS.dark_mode ? node_hovered_color_dark : node_hovered_color_white;

    update();
}


void CanvasWidget::UpdateFrame() {
    const qreal currentTime = QDateTime::currentMSecsSinceEpoch() / 1000.0;
    const qreal deltaTime = (currentTime - last_frame_time);
    last_frame_time = currentTime;
    SetSettings();

    if (!dragging) {
        velocity_x *= std::pow(friction, deltaTime * 60);
        velocity_y *= std::pow(friction, deltaTime * 60);

        if (std::abs(velocity_x) < 0.1) velocity_x = 0.0;
        if (std::abs(velocity_y) < 0.1) velocity_y = 0.0;

        offset_x += velocity_x * deltaTime * 60;
        offset_y += velocity_y * deltaTime * 60;

        if (scale_velocity != 0.0) {
            qreal oldScale = scale;
            scale_velocity *= std::pow(scale_friction, deltaTime * 60);
            qreal newScale = scale + scale_velocity * deltaTime * 60;

            if (std::abs(scale_velocity) < 0.0001) scale_velocity = 0;
            else {
                if (newScale >= min_scale && newScale <= max_scale) {
                    qreal scaleFactor = newScale / oldScale;

                    qreal centerX = this->width() / 2.0;
                    qreal centerY = this->height() / 2.0;

                    offset_x = centerX - (centerX - offset_x) * scaleFactor;
                    offset_y = centerY - (centerY - offset_y) * scaleFactor;

                    scale = newScale;
                } else {
                    scale_velocity = 0;
                }
            }
        }

        update();
    }
}



void CanvasWidget::paint(QPainter *painter) {
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

    painter->fillRect(boundingRect(), background_color);

    painter->save();

    painter->translate(offset_x, offset_y);
    painter->scale(scale / dpr, scale / dpr);

    DrawGrid(*painter, "dot");
    DrawGraph(*painter);
    if (SETTINGS.is_outline_mode and dragging && !vertices_dragging) DrawSelection(*painter);

    painter->restore();
}


void CanvasWidget::mousePressEvent(QMouseEvent *event) {
    HandlePressed(event);
}

void CanvasWidget::mouseMoveEvent(QMouseEvent *event) {
    HandlePositionChanged(event->scenePosition().toPoint());
}

void CanvasWidget::mouseReleaseEvent(QMouseEvent *event) {
    HandleReleased(event);
}

void CanvasWidget::HandlePressed(const QMouseEvent *event) {
    dragging = true;
    start_x = last_x = event->scenePosition().x();
    start_y = last_y = event->scenePosition().y();

    if (vertices_selected) {
        qreal delta = 10 / scale;
        vertices_dragging = false;
        for (auto &node : *nodes_ptr)
            if (node.is_selected)
                if (abs(node.x - (event->scenePosition().x() - offset_x) / scale) <= delta &&
                    abs(node.y - (event->scenePosition().y() - offset_y) / scale) <= delta)
                    vertices_dragging = true;
    }
}

void CanvasWidget::SelectEdge(const QMouseEvent *event) {
    qreal x = (event->scenePosition().x() - offset_x) / scale;
    qreal y = (event->scenePosition().y() - offset_y) / scale;
    qreal delta = 6 / scale;
    qreal min_delta = 100;
    qreal min_delta_debug_x = 1000;
    qreal min_delta_debug_y = 1000;
    const Edge *edge_to_select = nullptr;

    for (auto &edge_points : edge_draw_points) {
        for (auto &point : edge_points.points) {
            qreal edge_x = point.x();
            qreal edge_y = point.y();

            if (abs(x - edge_x) < min_delta_debug_x) {
                min_delta_debug_x = abs(x - edge_x);
            }
            if (abs(y - edge_y) < min_delta_debug_y) {
                min_delta_debug_y = abs(y - edge_y);
            }
            if (abs(edge_x - x) < delta &&
                abs(edge_y - y) < delta &&
                abs(edge_x - x) + abs(edge_y - y) < min_delta) {
                min_delta = abs(edge_x - x) + abs(edge_y - y);
                edge_to_select = edge_points.edge;
            }
        }
    }

    if (edge_to_select != nullptr) {
        edge_to_select->is_selected = !edge_to_select->is_selected;
        if (edge_to_select->is_selected) this->edges_selected += 1;
        else this->edges_selected -= 1;
        this->edges_selected_updated = true;
        this->selected_edge = edge_to_select;
    }
}

void CanvasWidget::DeselectEdges() {
    for (const auto &group_edge : *edges_ptr) {
        for (auto &edge : group_edge.edges) {
            if (edge.is_selected) {
                edge.is_selected = !edge.is_selected;
                this->edges_selected -= 1;
                this->edges_selected_updated = true;
            }
        }
    }
}

const Edge* CanvasWidget::GetSelectedEdge() {
    return selected_edge;
}

QString CanvasWidget::GetSelectedEdgeGroupName() {
    for (const auto &group_edge : *edges_ptr) {
        for (auto &edge : group_edge.edges) {
            if (edge.is_selected) {
                return !group_edge.name.empty() ? group_edge.name.data() : "";
            }
        }
    }

    return "";
}

void CanvasWidget::CalculateDrawPoints() {
    this->draw_points_set = false;
}

void CanvasWidget::SelectVertices(const QRectF &rect) {
    qreal left_x = rect.left();
    qreal top_y = rect.top();
    qreal right_x = rect.right();
    qreal bottom_y = rect.bottom();

    if (right_x < left_x) std::swap(left_x, right_x);
    if (top_y > bottom_y) std::swap(top_y, bottom_y);

    vertices_selected = false;

    for (auto &node : *this->nodes_ptr) {
        if (node.x >= left_x && node.x <= right_x && node.y >= top_y && node.y <= bottom_y) {
            node.is_selected = !node.is_selected;
        }

        vertices_selected = !vertices_selected ? node.is_selected : vertices_selected;
    }

    vertices_selected_updated = true;

    selection_rect = QRectF();
}

void CanvasWidget::DeselectVertices() {
    for (auto &node : *this->nodes_ptr) node.is_selected = false;
    vertices_selected = false;
    vertices_selected_updated = true;
}

void CanvasWidget::ChangeVertexColor(QColor &color) const {
    for (auto &node : *this->nodes_ptr) {
        if (node.is_selected) {
            node.is_default_color = false;
            node.color = color;
        }
    }
}


void CanvasWidget::HandleReleased(const QMouseEvent *event) {
    if (vertices_dragging) {
        draw_points_set = false;
        vertices_dragging = false;
    }

    if (abs(start_x - last_x) < 1 && abs(start_y - last_y) < 1) {
        DeselectVertices();
        DeselectEdges();
        SelectEdge(event);
    }

    if (SETTINGS.is_outline_mode) {
        SelectVertices(selection_rect);
    }

    dragging = false;
}

void CanvasWidget::SetData() {
    this->nodes_ptr = PAGE_MANAGER.GetNodes();
    this->edges_ptr = PAGE_MANAGER.GetEdges();
}

void CanvasWidget::DrawSelection(QPainter &painter) {
    painter.setBrush(selection_color);
    painter.setPen(Qt::NoPen);
    painter.setOpacity(0.5);

    qreal width = (last_x - start_x) / scale;
    qreal height = (last_y - start_y) / scale;

    QRectF rect((start_x - offset_x) / scale, (start_y - offset_y) / scale, width, height);
    selection_rect = rect;

    painter.drawRect(rect);

    painter.setOpacity(1.0);
}

void CanvasWidget::DrawGrid(QPainter &painter, const QString &style) {
    QColor grid_color = Qt::gray;

    if (style == "dot") {
        painter.setBrush(grid_color);
        painter.setPen(Qt::transparent);

        qreal grid_size = 50.0;
        qreal dot_radius = 2 / scale;

        qreal start_x = std::floor(-offset_x / scale / grid_size) * grid_size;
        qreal start_y = std::floor(-offset_y / scale / grid_size) * grid_size;


        for (qreal x = start_x; x < width() / scale - offset_x / scale; x += grid_size) {
            for (qreal y = start_y; y < height() / scale - offset_y / scale; y += grid_size) {
                QRectF ellipse(x, y, dot_radius, dot_radius);
                painter.drawEllipse(ellipse);
            }
        }
    }
}

std::vector<QPointF> GenerateArcPoints(const QRectF &rect, int startAngle, int spanAngle, int pointCount = 100) {
    std::vector<QPointF> points;
    points.reserve(pointCount);

    // Convert angles from 16th-of-a-degree units to radians
    double startRadians = -startAngle * M_PI / (180.0 * 16);
    double spanRadians = -spanAngle * M_PI / (180.0 * 16);

    // Center and radii of the arc
    qreal centerX = rect.center().x();
    qreal centerY = rect.center().y();
    qreal radiusX = rect.width() / 2;
    qreal radiusY = rect.height() / 2;

    // Generate points along the arc
    for (int i = 0; i < pointCount; ++i) {
        double t = static_cast<double>(i) / (pointCount - 1);  // t in [0, 1]
        double angle = startRadians + t * spanRadians;

        // Parametric equation for an ellipse
        double x = centerX + radiusX * std::cos(angle);
        double y = centerY + radiusY * std::sin(angle);

        points.emplace_back(x, y);
    }

    return points;
}

std::vector<QPointF> GenerateLinePoints(const QPointF &P_start, const QPointF &P_end, int pointCount = 50) {
    std::vector<QPointF> points;
    points.reserve(pointCount);

    for (int i = 0; i < pointCount; ++i) {
        double t = static_cast<double>(i) / (pointCount - 1);  // t in [0, 1]
        double x = P_start.x() + t * (P_end.x() - P_start.x());
        double y = P_start.y() + t * (P_end.y() - P_start.y());
        points.emplace_back(x, y);
    }

    return points;
}

void CanvasWidget::DrawGraph(QPainter &painter) {
    SetData();

    if (!draw_points_set) {
        edge_draw_points = std::vector<EdgeDrawPoints>();
    }

    for (const auto &group_edge : *edges_ptr) {
        for (auto &edge : group_edge.edges) {
            auto from_node = &((*nodes_ptr)[edge.from]);
            auto to_node = &((*nodes_ptr)[edge.to]);

            if (from_node && to_node) {
                QColor edge_color = node_connection_color;
                if (!group_edge.is_default_color)
                    edge_color = group_edge.color;
                if (edge.is_selected) {
                    edge_color = QColor(edge_selection_color);
                }
                painter.setPen(QPen(edge_color, SETTINGS.edges_width / scale));
                painter.setBrush(edge_color);
                const QPointF P_start(from_node->x, from_node->y);
                const QPointF P_end(to_node->x, to_node->y);

                if (group_edge.type == EdgeType::ET_BCurve) {
                    auto data = std::get<BCurveData>(group_edge.edge_type_data);

                    QPointF direction = P_end - P_start;
                    double length = std::hypot(direction.x(), direction.y());

                    QPointF dir_unit = direction / length;
                    QPointF perp_unit(-dir_unit.y(), dir_unit.x());
                    QPointF P_ref1 = P_start
                                    + data.along1 * direction
                                    + data.across1 * length * perp_unit;

                    QPointF P_ref2 = P_end
                                    - data.along2 * direction
                                    - data.across2 * length * perp_unit;
                    std::vector<QPointF> points = DrawBezierCurve(&painter, P_start, P_ref1, P_ref2, P_end, 200);
                    if (!draw_points_set) {
                        edge_draw_points.emplace_back(EdgeDrawPoints{&edge, points});
                    }
                } else if (group_edge.type == EdgeType::ET_Arc) {
                    auto ratio = std::get<ArcData>(group_edge.edge_type_data).ratio;

                    qreal x1 = from_node->x;
                    qreal y1 = from_node->y;
                    qreal x2 = to_node->x;
                    qreal y2 = to_node->y;

                    qreal dx = x2 - x1;
                    qreal dy = y2 - y1;
                    qreal chord_len = std::hypot(dx, dy);

                    if (chord_len < 1e-6 || ratio < 1e-6) {
                        painter.drawLine(P_start, P_end);
                        continue;
                    }

                    qreal normal_x = -dy / chord_len;
                    qreal normal_y = dx / chord_len;

                    qreal height = ratio * chord_len;
                    qreal radius = (chord_len * chord_len + 4 * height * height) / (8 * height);

                    qreal mid_x = (x1 + x2) / 2;
                    qreal mid_y = (y1 + y2) / 2;
                    qreal center_x = mid_x + normal_x * (radius - height);
                    qreal center_y = mid_y + normal_y * (radius - height);

                    qreal start_angle = std::atan2(y1 - center_y, x1 - center_x);
                    qreal end_angle = std::atan2(y2 - center_y, x2 - center_x);

                    int start = static_cast<int>(-start_angle * 180.0 / M_PI * 16);
                    int end = static_cast<int>(-end_angle * 180.0 / M_PI * 16);
                    int span = end - start;

                    if (span > 0)
                        span -= 360 * 16;

                    QRectF rect(center_x - radius, center_y - radius, 2 * radius, 2 * radius);
                    painter.drawArc(rect, start, span);
                    if (!draw_points_set) {
                        std::vector<QPointF> points = GenerateArcPoints(rect, start, span, 200);
                        edge_draw_points.emplace_back(EdgeDrawPoints{&edge, points});
                    }
                } else {
                    painter.drawLine(P_start, P_end);
                    if (!draw_points_set) {
                        std::vector<QPointF> points = GenerateLinePoints(P_start, P_end, 200);
                        edge_draw_points.emplace_back(EdgeDrawPoints{&edge, points});
                    }
                }

                if (edge.is_directed)
                    DrawArrowHead(painter, P_start, P_end, edge.is_selected ? edge_selection_color : edge_color, 10, scale);
            }
        }
    }
    draw_points_set = true;

    uint16_t node_index = 0;
    for (auto &node : *nodes_ptr) {
        QColor cur_node_color = node.is_default_color ? node_color : node.color;
        painter.setOpacity(1.0);
        painter.setPen(Qt::transparent);
        if (node.is_selected) {
            painter.setPen(QPen(node_hovered_color, 6.0 / scale));
        } else painter.setPen(Qt::NoPen);

        painter.setBrush(cur_node_color);

        if (node.type == 0 || node.type == 1) {
            qreal radius = node.is_selected ? (SETTINGS.nodes_radius + 2) / scale : SETTINGS.nodes_radius / scale;
            QRectF ellipse(node.x - radius, node.y - radius, 2 * radius, 2 * radius);

            if (node.type == 0) {
                painter.drawEllipse(ellipse);
            } else if (node.type == 1) {
                painter.setBrush(background_color);
                painter.drawEllipse(ellipse);

                painter.setBrush(Qt::NoBrush);
                painter.setPen(QPen(cur_node_color, SETTINGS.nodes_radius / scale));
                painter.drawEllipse(ellipse);
            }
        } else if (node.type == 2) {
            qreal diff = node.is_selected ? (SETTINGS.nodes_radius + 6) / scale : (SETTINGS.nodes_radius + 4) / scale;
            QRectF rect(node.x - diff, node.y - diff, 2 * diff, 2 * diff);
            painter.drawRect(rect);
        }
        painter.setPen(Qt::NoPen);

        if (scale < 1.0) {
            painter.setOpacity(std::max(0.0, (scale - 0.8) / 0.8));
        } else {
            painter.setOpacity(1.0);
        }

        if (SETTINGS.show_label_text) {
            painter.setPen(text_color);
            painter.setFont(QFont("monospace", SETTINGS.font_size / (SETTINGS.const_font_size ? 1 : scale)));
            painter.drawText(node.x, node.y - 18.0, QString::number(node_index));
        }

        node_index++;
    }
}




void CanvasWidget::ResetNodes() {
    for (auto &node : *nodes_ptr) {
        node.x = node.x_origin;
        node.y = node.y_origin;
    }

    vertices_moved = false;
    vertices_moved_updated = true;
}

void CanvasWidget::HandlePositionChanged(const QPointF &pos) {
    last_mouse_x = pos.x();
    last_mouse_y = pos.y();

    if (dragging) {
        qreal dx = pos.x() - last_x;
        qreal dy = pos.y() - last_y;
        if (vertices_dragging) {
            for (auto &node : *nodes_ptr)
                if (node.is_selected) {
                    if (!vertices_moved) {
                        vertices_moved = true;
                        vertices_moved_updated = true;
                    }
                    node.x = node.x + static_cast<int32_t>(dx / scale);
                    node.y = node.y + static_cast<int32_t>(dy / scale);
                }
        } else if (!SETTINGS.is_outline_mode) {
            offset_x += dx;
            offset_y += dy;
            velocity_x = dx;
            velocity_y = dy;
        }

        last_x = pos.x();
        last_y = pos.y();
    }
}

void CanvasWidget::wheelEvent(QWheelEvent *event) {
    HandleWheel(event->angleDelta());
}

void CanvasWidget::HandleWheel(QPoint delta) {
    qreal scale_change = delta.y() * 0.0002;
    qreal new_scale = std::max(min_scale, std::min(max_scale, scale + scale_change));

    if (new_scale != scale) {
        qreal mouseX = last_mouse_x;
        qreal mouseY = last_mouse_y;
        qreal scaleFactor = new_scale / scale;

        offset_x = mouseX - (mouseX - offset_x) * scaleFactor;
        offset_y = mouseY - (mouseY - offset_y) * scaleFactor;
        scale = new_scale;
        scale_velocity += scale_change;

    }
}

CanvasWidget::~CanvasWidget() {
}
