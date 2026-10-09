#pragma once

#include "ALTypes.h"
#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <cmath>
#include "GraphTopologies.h"
#include <QDateTime>
#include <QQuickPaintedItem>
#include <QQuickWindow>
#include <QSvgGenerator>

struct EdgeDrawPoints {
    const Edge *edge;
    std::vector<QPointF> points;
};

class CanvasWidget : public QQuickPaintedItem  {
    Q_OBJECT
public:
    bool draw_points_set = false;
    explicit CanvasWidget(QQuickPaintedItem *parent = nullptr);

    uint16_t edges_selected = 0;
    bool edges_selected_updated = false;
    bool vertices_selected = false;
    bool vertices_selected_updated = false;
    bool vertices_dragging = false;
    bool vertices_moved = false;
    bool vertices_moved_updated = false;

    const Edge* GetSelectedEdge();
    QString GetSelectedEdgeGroupName();

    void ChangeVertexColor(QColor &color) const;

    void DeselectEdges();

    void DeselectVertices();

    void DrawCanvasForSave();

    void CalculateDrawPoints();

    void ResetNodes();

    ~CanvasWidget() override;

protected:
    qreal offset_x = 0.0;
    qreal offset_y = 0.0;
    qreal velocity_x = 0.0;
    qreal velocity_y = 0.0;
    qreal friction = 0.92;
    qreal scale = 1.0;
    qreal scale_velocity = 0.0;
    qreal scale_friction = 0.92;
    qreal min_scale = 0.25;
    qreal max_scale = 10.0;
    qreal last_mouse_x = 0.0;
    qreal last_mouse_y = 0.0;
    qreal last_x = 0.0;
    qreal last_y = 0.0;
    qreal start_x = 0.0;
    qreal start_y = 0.0;

    bool dragging = false;

    qreal dpr = window() ? window()->devicePixelRatio() : 1.0;

    qreal last_frame_time = static_cast<double>(QDateTime::currentMSecsSinceEpoch()) / 1000.0;

    QRectF selection_rect = QRectF();

    QColor node_color_dark = QColor(0xbfbfbf);
    QColor node_color_white = QColor(0xbfbfbf);

    QColor node_connection_color_dark = QColor(0xbfbfbf);
    QColor node_connection_color_white = QColor(0xbfbfbf);

    QColor node_hovered_color_dark = QColor(0x6699ff);
    QColor node_hovered_color_white = QColor(0x6699ff);

    QColor background_color_dark = QColor(0x1e1e1e);
    QColor background_color_white = Qt::white;

    QColor grid_color_dark = QColor(0x484848);
    QColor grid_color_white = QColor(0xc3c3c3);

    QColor text_color_dark = QColor(0x848484);
    QColor text_color_white = QColor(0x848484);

    QColor selection_color_dark = QColor(Qt::darkGray);
    QColor selection_color_white = QColor(Qt::lightGray);

    QColor edge_selection_color = QColor(Qt::green);

    QColor node_color = node_color_dark;
    QColor node_connection_color = node_connection_color_dark;
    QColor background_color = background_color_dark;
    QColor grid_color = grid_color_dark;
    QColor text_color = text_color_dark;
    QColor selection_color = selection_color_dark;
    QColor node_hovered_color = node_hovered_color_dark;

    VertexLayout *nodes_ptr;
    std::vector<EdgeGroup> *edges_ptr;

    std::vector<EdgeDrawPoints> edge_draw_points;

    const Edge *selected_edge{};

    void SetData();
    void DrawGrid(QPainter &painter, const QString &style);
    void DrawGraph(QPainter &painter);
    void DrawSelection(QPainter &painter);

    void SelectVertices(const QRectF &rect);


    void SelectEdge(const QMouseEvent *event);

    void HandlePositionChanged(const QPointF &pos);
    void HandlePressed(const QMouseEvent* event);
    void HandleReleased(const QMouseEvent *event);
    void HandleWheel(QPoint delta);
    void SetSettings();

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent* event) override;
    void paint(QPainter *painter) override;

public slots:
    void UpdateFrame();
};


class TempCanvas : public QWidget {

public:
    explicit TempCanvas(QWidget *parent = nullptr);
    QImage DrawGraph(QSvgGenerator *generator = nullptr);
    QImage DrawCanvas(QSvgGenerator *generator = nullptr);

private:

    VertexLayout *nodes_ptr;
    std::vector<EdgeGroup> *edges_ptr;
};
